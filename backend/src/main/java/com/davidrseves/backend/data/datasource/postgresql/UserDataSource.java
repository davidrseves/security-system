package com.davidrseves.backend.data.datasource.postgresql;

import com.davidrseves.backend.data.entities.UserEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import java.util.Optional;
import java.util.UUID;


public interface UserDataSource extends JpaRepository<UserEntity, UUID> {

    // Spring Data JPA automáticamente implementa este método si sigues la convención de nombres
    Optional<UserEntity> findByEmail(String email);
}
