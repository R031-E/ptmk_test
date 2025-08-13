#include <iostream>
#include <random>
#include <vector>
#include "employee.h"

std::chrono::year_month_day parseDate(const std::string& date_str) {
    std::istringstream ss(date_str);
    int y, m ,d;
    char delimiter;
    ss >> y >> delimiter >> m >> delimiter >> d;
    return {std::chrono::year(y), std::chrono::month(m), std::chrono::day(d)};
}

std::string generateRandomString(int length) {
    std::string str;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 25);
    for (int i = 0; i < length; ++i) {
        str += 'a' + dist(gen);
    }
    return str;
}

std::string generateRandomName(char start_letter = 0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_letter(0, 25);
    std::uniform_int_distribution<int> dist_len(5, 10);
    if (start_letter == 0) {
        start_letter = 'A' + dist_letter(gen);
    }
    std::string surname(1, start_letter);
    surname += generateRandomString(dist_len(gen) - 1);
    std::string name(" ");
    name += 'A' + dist_letter(gen);
    name += generateRandomString(dist_len(gen) - 1);
    std::string patronymic(" ");
    patronymic += 'A' + dist_letter(gen);
    patronymic += generateRandomString(dist_len(gen) - 1);
    return surname + name + patronymic;
}

std::string generateRandomSex() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(gen) == 0 ? "Male" : "Female";
}

std::chrono::year_month_day generateRandomDate() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_year(1950, 2000);
    std::uniform_int_distribution<int> dist_month(1, 12);
    std::uniform_int_distribution<int> dist_day(1, 28);
    return {std::chrono::year(dist_year(gen)), std::chrono::month(dist_month(gen)), std::chrono::day(dist_day(gen))};
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ptmk_test <mode> [params]" << std::endl;
        return 1;
    }

    pqxx::connection conn("user=postgres password=admin port=5432 dbname=ptmk_test target_session_attrs=read-write");
    if (!conn.is_open()) {
        std::cerr << "Failed to connect to the database" << std::endl;
        return 1;
    }

    Employee::setDB(&conn);
    int mode = std::stoi(argv[1]);
    switch (mode) {
        case 1: {
            pqxx::work w(conn);
            std::string create_query = R"(
                    DROP TABLE IF EXISTS employees;
                    DROP TYPE IF EXISTS sex_type;
                    CREATE TYPE sex_type AS ENUM ('Male', 'Female');
                    CREATE TABLE employees (
                        id SERIAL PRIMARY KEY,
                        full_name VARCHAR(255) NOT NULL,
                        birth_date DATE NOT NULL,
                        sex sex_type NOT NULL
                    );

                )";
            w.exec(create_query);
            w.commit();
            break;
        }
        case 2: {
            if (argc != 5) {
                std::cerr << "Usage: ptmk_test 2 \"Full Name\" YYYY-MM-DD Sex" << std::endl;
                return 1;
            }
            std::string full_name = argv[2];
            std::chrono::year_month_day birth_date = parseDate(argv[3]);
            std::string sex = argv[4];
            Employee employee(full_name, birth_date, sex);
            employee.saveToDB();
            break;
        }
        case 3: {
            pqxx::work w(conn);
            pqxx::result res = w.exec("SELECT DISTINCT ON (full_name, birth_date) full_name, birth_date, sex FROM employees ORDER BY full_name, birth_date");
            w.commit();
            for (pqxx::row row : res) {
                std::string name = row[0].as<std::string>();
                std::string date_str = row[1].as<std::string>();
                std::string sex = row[2].as<std::string>();
                std::chrono::year_month_day birth_date = parseDate(date_str);
                Employee temp(name, birth_date, sex);
                int age = temp.calculateAge();
                std::cout << "Name: " << name << ", Birth Date: " << date_str << ", Sex: " << sex << ", Age: " << age << std::endl;
            }
            break;
        }
        case 4: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<Employee> emps;
            emps.reserve(1000000);
            for (int i = 0; i < 1000000; ++i) {
                std::string name = generateRandomName();
                std::string sex = generateRandomSex();
                std::chrono::year_month_day date = generateRandomDate();
                emps.emplace_back(name, date, sex);
            }
            Employee::batchSave(emps);
            emps.clear();
            for (int i = 0; i < 100; ++i) {
                std::string name = generateRandomName('F');
                std::string sex = "Male";
                std::chrono::year_month_day date = generateRandomDate();
                emps.emplace_back(name, date, sex);
            }
            Employee::batchSave(emps);
            pqxx::work w(conn);
            break;
        }
        case 5: {
            pqxx::work w(conn);
            w.exec("ANALYZE employees;");
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            pqxx::result res = w.exec("SELECT full_name, birth_date, sex FROM employees WHERE sex = 'Male' AND full_name LIKE 'F%'");
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            w.commit();
            for (pqxx::row row : res) {
                std::string name = row[0].as<std::string>();
                std::string date_str = row[1].as<std::string>();
                std::string sex = row[2].as<std::string>();
                std::cout << "Name: " << name << ", Birth Date: " << date_str << ", Sex: " << sex << std::endl;
            }
            std::cout << "SQL execution time: " << diff.count() << " seconds" << std::endl;
            break;
        }
    }
    return 0;
}