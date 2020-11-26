#include "openssl_common.h"

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

namespace hpke {

template<>
void
typed_delete(EVP_CIPHER_CTX* ptr)
{
  EVP_CIPHER_CTX_free(ptr);
}

template<>
void
typed_delete(EVP_PKEY_CTX* ptr)
{
  EVP_PKEY_CTX_free(ptr);
}

template<>
void
typed_delete(EVP_MD_CTX* ptr)
{
  EVP_MD_CTX_free(ptr);
}

template<>
void
typed_delete(EVP_PKEY* ptr)
{
  EVP_PKEY_free(ptr);
}

template<>
void
typed_delete(BIGNUM* ptr)
{
  BN_free(ptr);
}

template<>
void
typed_delete(EC_POINT* ptr)
{
  EC_POINT_free(ptr);
}

template<>
void
typed_delete(EC_KEY* ptr)
{
  EC_KEY_free(ptr);
}

template<>
void
typed_delete(X509* ptr)
{
  X509_free(ptr);
}

template<>
void
typed_delete(AUTHORITY_KEYID* ptr)
{
  AUTHORITY_KEYID_free(ptr);
}

///
/// Map OpenSSL errors to C++ exceptions
///

std::runtime_error
openssl_error()
{
  auto code = ERR_get_error();
  return std::runtime_error(ERR_error_string(code, nullptr));
}

} // namespace hpke
