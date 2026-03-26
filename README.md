Table Userpassword:
    
PassID | User(Foreign Key) | Loginname | Passwort (AES) encrypted | IV (initializing vector)

Table Logindata:
Loginname | Hash | Salt (in Base64)
    
Encryption Key = Masterlogin + Masterpassword 

Dependencies for Debian/Ubuntu:
sudo apt install qt6-base-dev
sudo apt install libcrypto++-dev
sudo apt install sqlite3

