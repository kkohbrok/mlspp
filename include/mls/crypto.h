#pragma once

#include <mls/common.h>
#include <openssl/evp.h>
#include <tls/tls_syntax.h>
#include <hpke/digest.h>
#include <hpke/hpke.h>
#include <hpke/signature.h>
#include <hpke/random.h>
#include <vector>

namespace mls {

/// Cipher suites
struct CipherSuite
{
  enum struct ID : uint16_t
  {
    unknown = 0x0000,
    X25519_AES128GCM_SHA256_Ed25519 = 0x0001,
    P256_AES128GCM_SHA256_P256 = 0x0002,
    X25519_CHACHA20POLY1305_SHA256_Ed25519 = 0x0003,
    X448_AES256GCM_SHA512_Ed448 = 0x0004,
    P521_AES256GCM_SHA512_P521 = 0x0005,
    X448_CHACHA20POLY1305_SHA512_Ed448 = 0x0006,
  };

  CipherSuite();
  CipherSuite(ID id_in);
  CipherSuite(const CipherSuite& other);
  CipherSuite(CipherSuite&& other);
  CipherSuite& operator=(const CipherSuite& other);

  ID id;
  std::unique_ptr<hpke::HPKE> hpke;
  std::unique_ptr<hpke::Digest> digest;
  std::unique_ptr<hpke::Signature> sig;

  bytes expand_with_label(const bytes& secret,
                          const std::string& label,
                          const bytes& context,
                          size_t size) const;

private:
  void reset(ID id_in);
};

tls::istream&
operator>>(tls::istream& str, CipherSuite& suite);

tls::ostream&
operator<<(tls::ostream& str, const CipherSuite& suite);

bool
operator==(const CipherSuite& lhs, const CipherSuite& rhs);

bool
operator!=(const CipherSuite& lhs, const CipherSuite& rhs);

extern const std::array<CipherSuite::ID, 6> all_supported_suites;

// Utilities
using hpke::random_bytes;

bool
constant_time_eq(const bytes& lhs, const bytes& rhs);

// HPKE Keys
struct HPKECiphertext
{
  bytes kem_output;
  bytes ciphertext;

  TLS_SERIALIZABLE(kem_output, ciphertext)
  TLS_TRAITS(tls::vector<2>, tls::vector<4>)
};

struct HPKEPublicKey
{
  bytes data;

  HPKECiphertext encrypt(CipherSuite suite,
                         const bytes& aad,
                         const bytes& pt) const;

  TLS_SERIALIZABLE(data)
  TLS_TRAITS(tls::vector<2>)
};

struct HPKEPrivateKey
{
  static HPKEPrivateKey generate(CipherSuite suite);
  static HPKEPrivateKey parse(CipherSuite suite, const bytes& data);
  static HPKEPrivateKey derive(CipherSuite suite, const bytes& secret);

  HPKEPrivateKey() = default;

  bytes data;
  HPKEPublicKey public_key;

  bytes decrypt(CipherSuite suite,
                const bytes& aad,
                const HPKECiphertext& ct) const;

  TLS_SERIALIZABLE(data, public_key)
  TLS_TRAITS(tls::vector<2>, tls::pass)

private:
  HPKEPrivateKey(bytes priv_data, bytes pub_data);
};

// Signature Keys
struct SignaturePublicKey
{
  bytes data;

  bool verify(const CipherSuite& suite,
              const bytes& message,
              const bytes& signature) const;

  TLS_SERIALIZABLE(data)
  TLS_TRAITS(tls::vector<2>)
};

struct SignaturePrivateKey
{
  static SignaturePrivateKey generate(CipherSuite suite);
  static SignaturePrivateKey parse(CipherSuite suite, const bytes& data);
  static SignaturePrivateKey derive(CipherSuite suite, const bytes& secret);

  bytes data;
  SignaturePublicKey public_key;

  bytes sign(const CipherSuite& suite, const bytes& message) const;

  TLS_SERIALIZABLE(data, public_key)
  TLS_TRAITS(tls::vector<2>, tls::pass)

private:
  SignaturePrivateKey(bytes priv_data, bytes pub_data);
};

} // namespace mls
