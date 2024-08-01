package com.davidrseves.backend.data.converters;

import com.davidrseves.backend.data.entities.UserEntity;
import com.davidrseves.backend.domain.models.User;
import org.springframework.stereotype.Component;


@Component
public class UserConverter {

    public UserEntity toEntity(User user) {
        if (user == null) {
            return null;
        }
        return new UserEntity(
                user.getId(),
                user.getEmail(),
                user.getPassword(),
                user.getCreatedAt()
        );
    }

    public User toModel(UserEntity entity) {
        if (entity == null) {
            return null;
        }
        return new User(
                entity.getId(),
                entity.getEmail(),
                entity.getPassword(),
                entity.getCreatedAt()
        );
    }
}
