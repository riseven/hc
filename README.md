# HC

*An encrypt/decrypt tool that I developed while in high school.*

DISCLAIMER: *This is not a serious cryptographic software, DO NOT USE for any sensitive data.
This is just my AMATEUR take at the field, programmed while I was at high school. It is really
easy to screw things up in the crypto field, so I really encourage you to use a serious 3rd
party library, which has been programmed by knowledgeable peopel in the field, and thoroughly tested.*

## Description

This is a symmetric key crypto algorithm. It can be used to crypt/decrypt files by providing a password.
The password is used to generate 6 polynomials. Then each byte is placed in a 6 dimensional matrix according
to the 6 polynomials. Finally the matrix is traversed lineally and each byte that is found is writed to
the output. The size of the matrix is limited by RAM, and the file is procesed by chunks if it is so big
that it does not fill entirely in the matrix. However, the matrix should remain sparsely filled at all times
to ensure a high throughput, since the efficiency of the algorithm fastly degrades when the matrix is becoming
full.

Thats basically it although there are also some more sugar to it, like for example each individual byte is also 
transformed to avoid being exposed to symbol statistical analysis of the encrypted text. 

