package com.davidrseves.backend.domain.exceptions;


public class UserNotFoundException extends RuntimeException {

    public UserNotFoundException() {
        super("User not found");
    }
}
