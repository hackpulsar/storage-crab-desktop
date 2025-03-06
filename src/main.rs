mod auth_services;
mod jwt_token_pair;

use actix_web::{web, App, HttpServer};
use sqlx::{Pool, Postgres};
use crate::jwt_token_pair::generate_shared_secret;

// Holds app state
pub struct AppState {
    secret: String,
    db: Pool<Postgres>,
}

// Database URL
const DB_URL: &str = env!("DATABASE_URL");

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    // Connecting to a database
    println!("Connecting to the database...");
    let pool = sqlx::postgres::PgPool::connect(DB_URL)
        .await
        .expect("DB connection failed");
    println!("Successfully connected to the database.");

    // Run migrations
    println!("Running database migrations...");
    match sqlx::migrate!().run(&pool).await {
        Ok(_) => (),
        Err(_) => panic!("Failed to run database migrate.")
    }
    println!("Migrations successful.");

    // Generating shared secret
    let secret = generate_shared_secret();

    // Starting a web server
    println!("Starting server.");
    HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(AppState {
                secret: secret.clone(),
                db: pool.clone()
            }))
            .service(auth_services::create_user)
            .service(auth_services::greet)
            .service(auth_services::login)
            .service(auth_services::refresh_token)
    })
    .bind(("127.0.0.1", 8080))?
    .run()
    .await
}