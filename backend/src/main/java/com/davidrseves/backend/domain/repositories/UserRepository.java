package com.davidrseves.backend.domain.repositories;

import com.davidrseves.backend.domain.models.User;
import java.util.List;
import java.util.Optional;
import java.util.UUID;


public interface UserRepository {
    User save(User user);   // Maneja tanto create como update
    Optional<User> findById(UUID id);
    List<User> findAll();
    void deleteById(UUID id);
    Optional<User> findByEmail(String email);
}
