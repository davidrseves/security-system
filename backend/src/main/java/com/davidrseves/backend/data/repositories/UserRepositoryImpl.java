package com.davidrseves.backend.data.repositories;

import com.davidrseves.backend.data.converters.UserConverter;
import com.davidrseves.backend.data.datasource.postgresql.UserDataSource;
import com.davidrseves.backend.data.entities.UserEntity;
import com.davidrseves.backend.domain.models.User;
import com.davidrseves.backend.domain.repositories.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;


@Repository
public class UserRepositoryImpl implements UserRepository {

    private final UserDataSource userDataSource;
    private final UserConverter userConverter;

    @Autowired
    public UserRepositoryImpl(UserDataSource userDataSource, UserConverter userConverter) {
        this.userDataSource = userDataSource;
        this.userConverter = userConverter;
    }

    @Override
    public User save(User user) {
        UserEntity entity = userConverter.toEntity(user);
        UserEntity savedEntity = userDataSource.save(entity);
        return userConverter.toModel(savedEntity);
    }

    @Override
    public Optional<User> findById(UUID id) {
        return userDataSource.findById(id).map(userConverter::toModel);
    }

    @Override
    public List<User> findAll() {
        return userDataSource.findAll().stream().map(userConverter::toModel).collect(Collectors.toList());
    }

    @Override
    public void deleteById(UUID id) {
        userDataSource.deleteById(id);
    }

    @Override
    public Optional<User> findByEmail(String email) {
        return userDataSource.findByEmail(email).map(userConverter::toModel);
    }
}
