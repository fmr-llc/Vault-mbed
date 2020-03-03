#include "deletedlib.h"

PrivateKey delPriv;
PublicKey clawback  = PrivateKey("Kzq8w6kkEXkWQN8CJSScLQfpkFUsJ6TqHHGBy1E6197byGahhDMb").publicKey();
PublicKey active    = PrivateKey("KzF2Wyvor6iyomL7svZTzf1RP7gNho8J3hmqAMg68HLiodhYFUmq").publicKey();
long locktime       = 9;
/*
'rec_staging':  CScript([
        OP_IF,
                lastblocktime, OP_NOP3, OP_DROP,
                pubkeys['staging'], OP_CHECKSIG,
        OP_ELSE,
                pubkeys['clawback'], OP_CHECKSIG,
        OP_ENDIF])
*/


string getAddress(){
	return delPriv.address();
}

PublicKey getPublicKey(){
	return delPriv.publicKey();
}

Script getScript(){
    return delPriv.publicKey().script();
}

Signature signMessage(uint8_t buf[32]){
    return delPriv.sign(buf);
}

/* FIXME temporary global */
extern FSHandler* fs_handler_ptr;

string constructTx(char* buf){
    Tx tx = Tx();
    tx.fromString(buf);

    for (size_t i = 0; i < tx.inputsNumber; i++){
        tx.signInput(i, delPriv);
    }

    FILE *f = fs_handler_ptr->open((char*)tx.txid().c_str(), 1);
    fprintf(f, "%s\n", tx.toString().c_str());
    fs_handler_ptr->close(f);

    deleteKey();
    return tx.txid();
}

void generateKey() {
	uint8_t randomBuffer[32];
    //filling random buffer with 32 bytes
    getRandomBuffer(randomBuffer, sizeof(randomBuffer));
    delPriv = PrivateKey(randomBuffer);
    for (int i = 0; i < 100; ++i){
    	bzero(randomBuffer, sizeof(randomBuffer));
    }

}

void deleteKey(){
	for (int i = 0; i < 100; ++i){
    	bzero(&delPriv, sizeof(delPriv));
    }

}
