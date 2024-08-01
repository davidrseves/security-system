package com.davidrseves.backend.presentation;

import com.davidrseves.backend.domain.models.User;
import com.davidrseves.backend.domain.usecases.UserUseCase;
import jakarta.validation.Valid;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.UUID;


@RestController
@RequestMapping("/api/users")
public class UserController {

    private final UserUseCase userUseCase;

    @Autowired
    public UserController(UserUseCase userUseCase) {
        this.userUseCase = userUseCase;
    }

    // Registrar nuevo usuario
    @PostMapping("/register")
    public ResponseEntity<UUID> registerUser(@Valid @RequestBody User user) {
        User createdUser = userUseCase.registerUser(user);
        return ResponseEntity.status(201).body(createdUser.getId());
    }

    // Inicio de sesión
    @PostMapping("/login")
    public ResponseEntity<UUID> loginUser(@Valid @RequestBody User user) {
        User loggedInUser = userUseCase.loginUser(user);
        return ResponseEntity.ok(loggedInUser.getId());
    }

    // Cambio de contraseña
    @PutMapping("/{id}/change-password")
    public ResponseEntity<UUID> changePassword(@PathVariable UUID id, @Valid @RequestBody User user) {
        User updatedUser = userUseCase.changePassword(id, user);
        return ResponseEntity.ok(updatedUser.getId());
    }

    // Eliminar cuenta
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteAccount(@PathVariable UUID id) {
        userUseCase.deleteAccount(id);
        return ResponseEntity.noContent().build();
    }
}
