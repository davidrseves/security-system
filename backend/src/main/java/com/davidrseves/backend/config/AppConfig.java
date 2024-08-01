package com.davidrseves.backend.config;

import com.davidrseves.backend.data.repositories.UserRepositoryImpl;
import com.davidrseves.backend.domain.repositories.UserRepository;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configurers.AbstractHttpConfigurer;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.util.matcher.AntPathRequestMatcher;


@Configuration
public class AppConfig {

    // Union de repositorios
    @Bean
    public UserRepository userRepository(UserRepositoryImpl userRepositoryImpl) {
        return userRepositoryImpl;
    }

    // Para el hash del password
    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }

    // Seguridad de los endpoints
    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
                .csrf(AbstractHttpConfigurer::disable)  // Desactivar CSRF para simplificar las pruebas de API
                .authorizeHttpRequests(auth -> auth
                        .requestMatchers(
                                new AntPathRequestMatcher("/api/users/register"),
                                new AntPathRequestMatcher("/api/users/login")
                        ).permitAll()
                        .anyRequest().authenticated()
                )
                .formLogin(AbstractHttpConfigurer::disable);  // Desactivar el formulario de login por defecto

        return http.build();
    }
}
