use actix_web::{get, post, web, HttpResponse, Responder};
use sqlx::{self, Row};
use serde::{Deserialize, Serialize};
use crate::AppState;

// Represents a user
#[derive(Serialize, Deserialize)]
pub struct User {
    email: String,
    username: String,
    password: String,
}

// Response to a create user request
#[derive(Serialize)]
struct CreateUserResponse {
    id: i32,
    user: User,
}

#[get("/api/greet/{id}")]
pub async fn greet(
    id: web::Path<i32>,
    data: web::Data<AppState>,
) -> impl Responder {
    let user_id = id.into_inner();

    // Look up username ith given ID
    let query = "select username from users where id = $1";
    let res = sqlx::query(query)
        .bind(user_id)
        .fetch_optional(&data.db)
        .await;

    // Print hello if found, otherwise print funny stuff
    match res {
        Ok(row) => {
            match row {
                Some(entry) => {
                    let username: String = entry.get("username");
                    format!("Hello, dear {username}!")
                },
                None => "You don't exist mate!".to_string()
            }
        },
        Err(_) => "Something went wrong searching for the user.".to_string()
    }
}

// Creates a new user in a database
#[post("/api/users/")]
pub async fn create_user(
    user: web::Json<User>,
    data: web::Data<AppState>
) -> impl Responder {
    // User JSON to User struct
    let user = user.into_inner();

    // Perform a query
    let query = "insert into users(email, username, password) values ($1, $2, $3) returning id";
    let res = sqlx::query(query)
        .bind(user.email.clone())
        .bind(user.username.clone())
        .bind(user.password.clone())
        .fetch_one(&data.db)
        .await;

    // If success, send back a JSON with the created user credentials and an ID.
    // Otherwise, send back an internal server error.
    match res {
        Ok(record) => HttpResponse::Ok().json(CreateUserResponse {
            id: record.get("id"),
            user: User {
                email: user.email,
                username: user.username,
                password: user.password,
            }
        }),
        Err(_) => HttpResponse::InternalServerError().body("Error while inserting user")
    }
}