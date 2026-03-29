#include <iostream>
#include <cryptopp/scrypt.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/base64.h>
#include <cryptopp/sha.h>
#include <cryptopp/pwdbased.h>
#include "cryptopp/rijndael.h"
#include "cryptopp/modes.h"
#include "model/loginhandler.hpp"
#include "model/user.hpp"
#include <format>


std::string EncodeToBase64(CryptoPP::SecByteBlock &dataword)
{
    std::string word_base64;
    CryptoPP::Base64Encoder encoder(NULL, false);
    encoder.Put(dataword, dataword.size(), true);
    encoder.MessageEnd();
    CryptoPP::word64 size = encoder.MaxRetrievable();
    if (size)
    {
        word_base64.resize(size);
        encoder.Get((CryptoPP::byte *)&word_base64[0], word_base64.size());
    }
    return word_base64;
}

CryptoPP::SecByteBlock DecodeBase64(std::string &word)
{
    CryptoPP::Base64Decoder decoder;
    decoder.Put((CryptoPP::byte *)word.data(), word.size(), true);
    decoder.MessageEnd();

    CryptoPP::word64 size = decoder.MaxRetrievable();

    CryptoPP::SecByteBlock decodedBytes(size);
    if (size > 0)
    {
        decoder.Get(decodedBytes.data(), decodedBytes.size());
    }
    return decodedBytes;
}

Loginhandler::Loginhandler()
{
}

User Loginhandler::CreateNewUser(const std::string &login, const std::string &password)
{
    // Generate a random salt
    CryptoPP::SecByteBlock salt = GenerateSalt(SALT_SIZE);

    // Generate a Hash out of the password and salt
    CryptoPP::SecByteBlock derived(HASH_SIZE);
    CryptoPP::byte *pass = (CryptoPP::byte *)password.data();
    size_t passlen = password.length();
    CryptoPP::Scrypt scrypt;
    scrypt.DeriveKey(derived, derived.size(), pass, passlen, salt.data(), SALT_SIZE);
    // Encode to base64
    std::string passwd_base64 = EncodeToBase64(derived);
    std::string salt_base64 = EncodeToBase64(salt);

    User newUser(login, passwd_base64, salt_base64);
    return newUser;
}

bool Loginhandler::CheckLoginCredentials(std::shared_ptr<DBHandler> dbHandler, std::string login, std::string password)
{
    // Get The data from the user and compare it with the parameters of the Function
    User userToAuth = dbHandler->GetMasterLoginData(login);

    // Take salt from DB and decode it to binary
    std::string salt = userToAuth.mSalt;
    CryptoPP::SecByteBlock saltDecoded = DecodeBase64(salt);

    // Generate the hash from the entered password and salt that is in the db
    CryptoPP::byte *pass = (CryptoPP::byte *)password.data();
    size_t passlength = password.length();
    CryptoPP::Scrypt scrypt;
    CryptoPP::SecByteBlock hashFromEnteredData(HASH_SIZE);
    scrypt.DeriveKey(hashFromEnteredData, hashFromEnteredData.size(), pass, passlength, saltDecoded.data(), saltDecoded.size());

    std::string hashInDBBase64 = userToAuth.mPassword;
    CryptoPP::SecByteBlock hashInDB = DecodeBase64(hashInDBBase64);

    if (hashInDB == hashFromEnteredData)
    {
        return true;
    }
    else
    {
        return false;
    }
}

CryptoPP::SecByteBlock Loginhandler::GenerateSalt(size_t size)
{
        // Generate a random salt
    CryptoPP::SecByteBlock salt(size);
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(salt, salt.size());
    return salt;
}

/**
 * Generates the Master Key with the given data. This ensures that no unauthorized users can see the password
 */
static CryptoPP::SecByteBlock DeriveMasterKey(std::string &inputdata, std::string &salt_base64)
{
    CryptoPP::SecByteBlock key(AES_KEYLENGTH);
    CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf;

    CryptoPP::SecByteBlock salt_binary = DecodeBase64(salt_base64);
    pbkdf.DeriveKey(key.data(), key.size(), 0, (CryptoPP::byte*)inputdata.data(), inputdata.size(), salt_binary.data(), salt_binary.size(), MASTER_KEY_ITERATIONS);
    return key;
}


/**
 * Generates a PasswordEntry which can contains all the encrypted data to be inserted into the database
 */
PasswordEntry Loginhandler::GenerateLoginEntry(std::string &userLogin, std::string &userPassword, std::string &userSalt, std::string &sourceLogin, std::string &sourcePassword, std::string &source)
{
    PasswordEntry entry;
    std::string userAndPassword = userLogin + userPassword;
    CryptoPP::SecByteBlock masterKey = DeriveMasterKey(userAndPassword, userSalt);

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock iv(IV_LENGTH);
    prng.GenerateBlock(iv, iv.size());

    std::string cipher;
    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption;
        encryption.SetKeyWithIV(masterKey, masterKey.size(), iv);
        
        CryptoPP::StringSource ss(sourcePassword, true,
            new CryptoPP::StreamTransformationFilter(encryption,
                new CryptoPP::StringSink(cipher)
            )
        );
    }
        // THE IV needs TO BE STORED IN THE DB
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::string iv_base64 = EncodeToBase64(iv);
    CryptoPP::SecByteBlock cipher_byteblock;
    cipher_byteblock.Assign((const CryptoPP::byte*)cipher.data(), cipher.size());
    std::string cipher_base64 = EncodeToBase64(cipher_byteblock); 


    entry.mUser = userLogin;
    entry.mLogin = sourceLogin;
    entry.mIV = iv_base64;
    entry.mPassword = cipher_base64;
    entry.mSource = source;
    return entry;
}

// Decrypt all Passwords in the vector in place. Can throw an error
void Loginhandler::DecryptAllPasswords(std::vector<PasswordEntry> &passwordEntries, std::string &masterLogin, std::string &masterPassword, std::string &salt_base64)
{
    //Create the masterpassword for encryption
    std::string userAndPassword = masterLogin + masterPassword;
    CryptoPP::SecByteBlock masterKey = DeriveMasterKey(userAndPassword, salt_base64);
    
    try
    {
        for(PasswordEntry &entry : passwordEntries)
        {
            CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decrypter;
            CryptoPP::SecByteBlock cipher_encoded_raw = DecodeBase64(entry.mPassword);
            std::string recovered;
            CryptoPP::SecByteBlock iv_byteblock = DecodeBase64(entry.mIV);
            decrypter.SetKeyWithIV(masterKey, masterKey.size(), iv_byteblock);

            CryptoPP::StringSource s(cipher_encoded_raw.data(), cipher_encoded_raw.size(), true, 
            new CryptoPP::StreamTransformationFilter(decrypter,
                new CryptoPP::StringSink(recovered)
            ) // StreamTransformationFilter
        ); // StringSource
        entry.mPassword = recovered;
        }
    }
    catch(const std::exception& e)
    {
        throw e;
    }
    
}