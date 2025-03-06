mod auth_services;

use actix_web::{web, App, HttpServer};
use sqlx::{Pool, Postgres};

// Database URL
const DB_URL: &str = env!("DATABASE_URL");

struct AppState {
    db: Pool<Postgres>,
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    // Connecting to a database
    let pool = sqlx::postgres::PgPool::connect(DB_URL)
        .await
        .expect("DB connection failed");

    // Run migrations
    match sqlx::migrate!().run(&pool).await {
        Ok(_) => (),
        Err(_) => panic!("Failed to run database migrate.")
    }

    // Starting a web server
    HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(AppState { db: pool.clone() }))
            .service(auth_services::create_user)
            .service(auth_services::greet)
    })
    .bind(("127.0.0.1", 8080))?
    .run()
    .await
}