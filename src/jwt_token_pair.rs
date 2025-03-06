use jsonwebtoken::{encode, EncodingKey, Header};
use serde::{Deserialize, Serialize};
use std::process::Command;

#[derive(Serialize, Deserialize)]
pub struct Claims {
    pub sub: String,        // subject (whom token refers to)
    pub exp: usize,         // token expiration date
    pub token_type: TokenType, // token type (access or refresh)
}

// Represents a token pair
#[derive(Serialize)]
pub struct JwtTokenPair {
    access_token: String,
    refresh_token: String,
}

// Token types
#[derive(Serialize, Deserialize, PartialEq)]
pub enum TokenType {
    Access,
    Refresh,
}

impl JwtTokenPair {
    pub fn generate_for(user_email: String, secret: String) -> Self {
        let expiration = chrono::Utc::now() + chrono::Duration::minutes(10);
        let access_claims = Claims {
            sub: user_email.clone(),
            exp: expiration.timestamp() as usize,
            token_type: TokenType::Access,
        };
        let refresh_claims = Claims {
            sub: user_email.clone(),
            exp: expiration.timestamp() as usize,
            token_type: TokenType::Refresh,
        };

        // Return a freshly generated token pair
        Self {
            access_token: encode(
                &Header::default(),
                &access_claims,
                &EncodingKey::from_secret(secret.as_bytes())
            ).unwrap(),
            refresh_token: encode(
                &Header::default(),
                &refresh_claims,
                &EncodingKey::from_secret(secret.as_bytes())
            ).unwrap()
        }
    }
}

// Generates shared secret using openssl
pub fn generate_shared_secret() -> String{
    String::from_utf8(Command::new("openssl")
        .arg("rand")
        .arg("-base64")
        .arg("32")
        .output()
        .expect("Failed to generate shared secret. Make sure openssl is installed.")
        .stdout
    )
    .unwrap()
    .trim()
    .to_string()
}
