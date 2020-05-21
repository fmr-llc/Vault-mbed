# Vault-mbed

This project is a prototype of a hardware Bitcoin vault using pre-signed transactions and securely-deleted private keys. The intended device is the STM32F469I-Disco dev board [1], but can be adapted to work with other dev boards/microcontrollers going forward.

## Disclaimer
Warning: The code present in this repo is still in development and is not production ready. Do not use it to hold funds on mainnet. 

## Background Info
This repo is an implementation of a Bitcoin vault. A **vault** is an on-chain mechanism used to lock up a set of UTXOs until they are unvaulted. The vault works by committing to a pre-determined set of restrictions on the coins, called a **covenant**. Upon withdrawing, or unvaulting, the coins are subject to these pre-determined spending conditions; either waiting out a time lock (P2TST)[2] and sending the funds to some external/hot wallet or redirecting the funds to a backup/deep cold wallet (P2RW)[3].

This repo is part of a larger codebase for our Bitcoin vault implementation. The code in this repo is for the custom firmware running on what we refer to as the **vault wallet**. The vault wallet is responsible for securely generating/deleting private keys, signing the pre-signed transaction, and some Protobuf message passing.

More information on the entirety of our Bitcoin vault implementation can be read here:
Vaults paper [3]
Covenants paper [4]
FIXME: link to manuscripts upon completion.

The origin of the deleted key vault idea can be found in Links [2].

## What's in this repo?
This repo contains the custom firmware aimed to run on the **vault wallet** hardware device as part of our Bitcoin vault. As mentioned above, the intended device is the STM32F469I-Disco dev board [1]. 

Along with a suite of code for the secure generation/deletion of keys, the vault wallet implements its own `Protobuf` message handler using the Trezor communications protocol. The counterpart in these messages will be what we are referring to as the **computer interface**. Currently, the computer interface is a modified version of Bitcoin Core's Hardware Wallet Interface (HWI) [5].

### Custom Protobuf Messages
As part of our design, we have implemented a set of custom Trezor Protobuf messages. The rough idea of these messages can be found below, but will likely be changed/expanded in the future.

```
message PrepareVault {
    string signThis = 1;
}
```

The first message in the vaulting process. The computer interface will send this message to the vault wallet to tell it to initiate the vaulting procedure. 

```
message PrepareVaultResponse {
    string address = 1;
    string redeemScript = 2;
    string sig(signThis) = 3;
}
```
After receiving the `PrepareVault` message, the vault wallet will generate a private key and its corresponding redeemScript for the upcoming transaction. Additionally, the vault wallet will sign `signThis` and return its signature for the possibility of authenticated messages. 

```
message FinalizeVault {
    string hex = 1;
}
```
After the computer interface constructs the vaulting transaction, it hands it off to the vault wallet to be signed.

```
message FinalizeVaultResponse {
    bool isDeleted = 1;
    string txid = 2;
}
```
The vault wallet will then respond with the txid of the newly created pre-signed transaction (P2TST)[2], along with confirmation that the private key is deleted The **covenant** is officially created and enforced on these coins in the form of the pre-signed transaction. 
There are considerations on where to store the pre-signed transaction, but for now, they will be stored in a filesystem on the vault wallet.

```
message UnvaultRequest{
    string txid = 1;
}
```
Upon a unvault request, the computer interface will send a message to the vault wallet containing the txid of the desired pre-signed transaction.

```
message UnvaultResponse{
    string hex = 1;
}
```
The vault wallet will then respond with the pre-signed transaction (P2TST)[2] hex whose txid corresponds to `txid` received in the `UnvaultRequest` message sent by the computer interface prior.

## Installing
Will be updated with future releases.

## Running
Will be updated with future releases.

## Debugging
Will be updated with future releases.

# Roadmap/Future Plans
In no particular order:

### Watch Tower ###
An implementation of a watch tower is necessary for this vault design to function. In its simplest form, a watch tower will notify the wallet owner of a theft attempt so he/she is able to recover funds before the timelock expires.

### Rust/C Implementation ###
For prototyping reasons, the language of choice was Python. However, in a production-level use case, Python may not be the best choice due to a whole laundry list of potential reasons. As a result, a C/Rust implementation will likely be on the roadmap as the project matures.

### Code Review ###
Nobody wants to lose their coins due to a software issue. Thus, this project (and projects like this) are going to require extensive code review and optimization to ensure that funds are not lost.

### Proposed OpCode-based Covenant mechanisms ###
Although slow but surely, Bitcoin is an ever changing codebase. New features will be added in the future that will expand Bitcoin's scripting functionalities. This likely will include expanding Bitcoin's ability to create/enforce covenants. We are not only aware of such mecahnisms, but actively evaluating them as well. 
It is likely that we will one day see a better covenant mechanism than deleted keys, and if so, we will likely explore converting our design to fit such a mechanism.
If interested, one such mechanism we are actively researching is Jeremy Rubin's BIP119 OP_CheckTemplateVerify design [6].

## Other Implementations
There is a sizeable amount of related work.
1. Bryan Bishop's (@Kanzure) python-vaults repo: https://github.com/kanzure/python-vaults
2. @JSwambo bitcoin-vault repo: https://github.com/JSwambo/bitcoin-vault
3. @kloaec's implementation re-vault: https://github.com/re-vault/revault-demo

## Links

.. [1] https://www.st.com/en/evaluation-tools/32f469idiscovery.html

.. [2] https://medium.com/@BobMcElrath/re-imagining-cold-storage-with-timelocks-1f293bfe421f

.. [3] vaults paper

.. [4] covenants paper

.. [5] https://github.com/fmr-llc/HWI

.. [6] https://utxos.org/
