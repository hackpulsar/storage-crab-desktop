create table users (
    id serial primary key,
    email varchar not null,
    username varchar not null,
    password varchar not null
)