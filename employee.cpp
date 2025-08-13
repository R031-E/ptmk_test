//
// Created by Nikita Maz on 10.08.2025.
//

#include "employee.h"

pqxx::connection* Employee::db_conn = nullptr;

void Employee::setDB(pqxx::connection* conn) {
    db_conn = conn;
}

int Employee::getId() const{
    return id;
}

void Employee::setFullName(const std::string& fullName) {
    m_fullName = fullName;
}

void Employee::setBirthDate(const std::chrono::year_month_day& birthDate) {
    m_birthDate = birthDate;
}

void Employee::setSex(const std::string& sex) {
    m_sex = sex;
}

const std::string& Employee::getFullName() const{
    return m_fullName;
}

std::chrono::year_month_day Employee::getBirthDate() const{
    return m_birthDate;
}

const std::string& Employee::getSex() const{
    return m_sex;
}
//Метод сохранения в БД использует обычный insert
int Employee::saveToDB() {
    if (db_conn == nullptr) return -1;
    pqxx::work w(*db_conn);
    std::string str_birthDate = std::format("{:%Y-%m-%d}", this->getBirthDate());
    std::string query = "INSERT INTO employees (full_name, birth_date, sex) VALUES (" +
                        w.quote(m_fullName) + ", " + w.quote(str_birthDate) + ", " + w.quote(m_sex) + ") RETURNING id";
    pqxx::row res = w.exec1(query);
    id = res[0].as<int>();
    w.commit();
    return id;
}

int Employee::calculateAge() const {
    auto now = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    std::chrono::year_month_day today{now};
    int age = static_cast<int>(today.year()) - static_cast<int>(m_birthDate.year());
    if (today.month() < m_birthDate.month() ||
        (today.month() == m_birthDate.month() && today.day() < m_birthDate.day())) {
        age--;
    }
    return age;
}
//Метод сохранения сразу множества объектов в бд использует stream
void Employee::batchSave(const std::vector<Employee>& employees) {
    if (db_conn == nullptr) return;
    pqxx::work w(*db_conn);
    pqxx::stream_to stream = pqxx::stream_to::table(w, {"public", "employees"}, {"full_name", "birth_date", "sex"});
    for (const auto& employee : employees) {
        std::string str_birthDate = std::format("{:%Y-%m-%d}", employee.getBirthDate());
        stream << std::make_tuple(employee.m_fullName, str_birthDate, employee.m_sex);
    }
    stream.complete();
    w.commit();
}