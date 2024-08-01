package com.davidrseves.backend.domain.usecases;

import com.davidrseves.backend.domain.models.User;
import com.davidrseves.backend.domain.repositories.UserRepository;
import com.davidrseves.backend.domain.exceptions.UserAlreadyExistsException;
import com.davidrseves.backend.domain.exceptions.UserNotFoundException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import java.util.UUID;


@Service
public class UserUseCase {

    private final UserRepository userRepository;
    private final PasswordEncoder passwordEncoder;

    @Autowired
    public UserUseCase(UserRepository userRepository, PasswordEncoder passwordEncoder) {
        this.userRepository = userRepository;
        this.passwordEncoder = passwordEncoder;
    }

    // Registrar nuevo usuario
    public User registerUser(User user) {
        if (userRepository.findByEmail(user.getEmail()).isPresent()) {
            throw new UserAlreadyExistsException();
        }

        String hashedPassword = passwordEncoder.encode(user.getPassword());
        user.setPassword(hashedPassword);
        return userRepository.save(user);
    }

    // Inicio de sesión
    public User loginUser(User user) {
        if (userRepository.findByEmail(user.getEmail()).isEmpty()) {
            throw new UserNotFoundException();
        }

        User foundUser = userRepository.findByEmail(user.getEmail()).get();

        if (passwordEncoder.matches(user.getPassword(), foundUser.getPassword())) {
            return foundUser;
        } else {
            throw new UserNotFoundException();
        }
    }

    // Cambio de contraseña
    public User changePassword(UUID id, User user) {
        if (userRepository.findById(id).isEmpty()) {
            throw new UserNotFoundException();
        }

        User existingUser = userRepository.findById(id).get();

        String hashedPassword = passwordEncoder.encode(user.getPassword());
        existingUser.setPassword(hashedPassword);
        return userRepository.save(existingUser);
    }

    // Eliminar cuenta
    public void deleteAccount(UUID id) {
        if (userRepository.findById(id).isEmpty()) {
            throw new UserNotFoundException();
        }

        userRepository.deleteById(id);
    }
}
