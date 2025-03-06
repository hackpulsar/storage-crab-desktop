use jsonwebtoken::{encode, EncodingKey, Header};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct Claims {
    sub: String,        // subject (whom token refers to)
    exp: usize,         // token expiration date
    token_type: String, // token type (access or refresh)
}

// Represents a token pair
#[derive(Serialize)]
pub struct JwtTokenPair {
    access_token: String,
    refresh_token: String,
}

impl JwtTokenPair {
    pub fn generate(user_email: String) -> Self {
        let expiration = chrono::Utc::now() + chrono::Duration::minutes(10);
        let access_claims = Claims {
            sub: user_email.clone(),
            exp: expiration.timestamp() as usize,
            token_type: "access".to_string(),
        };
        let refresh_claims = Claims {
            sub: user_email.clone(),
            exp: expiration.timestamp() as usize,
            token_type: "refresh".to_string(),
        };

        // Return a freshly generated token pair
        Self {
            access_token: encode(
                &Header::default(),
                &access_claims,
                &EncodingKey::from_secret(b"secret")
            ).unwrap(),
            refresh_token: encode(
                &Header::default(),
                &refresh_claims,
                &EncodingKey::from_secret(b"secret")
            ).unwrap()
        }
    }
}