//
// Created by Nikita Maz on 10.08.2025.
//

#ifndef PTMK_TEST_EMPLOYEE_H
#define PTMK_TEST_EMPLOYEE_H
#include <string>
#include <chrono>
#include <utility>
#include <pqxx/pqxx>

class Employee {
    private:
        int id;
        std::string m_fullName;
        std::chrono::year_month_day m_birthDate;
        std::string m_sex;
        static pqxx::connection* db_conn;
    public:
        Employee(std::string fullName, std::chrono::year_month_day birthDate, std::string sex) :
            m_fullName(std::move(fullName)), m_birthDate(birthDate), m_sex(std::move(sex)) {this->id = -1;};
        int getId() const;
        const std::string& getFullName() const;
        std::chrono::year_month_day getBirthDate() const;
        const std::string& getSex() const;
        void setFullName(const std::string& fullName);
        void setBirthDate(const std::chrono::year_month_day& birthDate);
        void setSex(const std::string& sex);
        int saveToDB();
        int calculateAge() const;
        static void setDB(pqxx::connection* conn);
        static void batchSave(const std::vector<Employee>& employees);
};


#endif //PTMK_TEST_EMPLOYEE_H