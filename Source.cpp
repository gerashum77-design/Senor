#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <chrono>
#include <sstream>
class ISenorBlackBox {
public:
    virtual ~ISenorBlackBox() = default;
    virtual std::string processHiddenPayload(const std::string& encryptedInput) = 0;
};
struct EditorTask {
    uint64_t id;
    std::string type; // "edit", "analyze", "refactor"
    std::string payload;
    bool isCompleted = false;
    std::string result;
};
class SenorTaskBot {
private: std::queue<std::shared_ptr<EditorTask>> taskQueue;
       std::map<uint64_t, std::shared_ptr<EditorTask>> archive;
       uint64_t nextTaskId = 1;
public:
    uint64_t assignTask(const std::string& type, const std::string& data) {
        auto task = std::make_shared<EditorTask>(EditorTask{ nextTaskId++, type, data, false, "" });
        taskQueue.push(task);
        archive[task->id] = task;
        return task->id;
    }
    bool hasTasks() const { return !taskQueue.empty(); }
    std::shared_ptr<EditorTask> getNextTask() {
        if (taskQueue.empty()) return nullptr;
        auto task = taskQueue.front();
        taskQueue.pop();
        return task;
    }
    void updateTaskStatus(uint64_t id, const std::string& result) {
        if (archive.find(id) != archive.end()) {
            archive[id]->isCompleted = true;
            archive[id]->result = result;
        }
    }
    void printTaskStatus(uint64_t id) const {
        auto it = archive.find(id);
        if (it != archive.end()) {
            std::cout << "[TaskBot] Задача #" << id << " [" << it->second->type << "] -> "
                << (it->second->isCompleted ? "ВЫПОЛНЕНО: " + it->second->result : "В ОЧЕРЕДИ")
                << std::endl;
        }
    }
};
class CryptoProvider {
public:
    static std::string obfuscate(const std::string& data) {
        std::string result = data;
        for (char& c : result) c ^= 0x5A;
        return result;
    }
};
class SenorBlackBoxImpl : public ISenorBlackBox {
private:
    std::string executeInternalAI(const std::string& rawInput) {
        if (rawInput.find("refactor") != std::string::npos) {
            return "[Senor AI] Оптимизация кода успешно завершена. Сложность снижена до O(1).";
        }
        else if (rawInput.find("edit") != std::string::npos) {
            return "[Senor AI] Текст отредактирован. Исправлены стилистические ошибки.";
        }
        return "[Senor AI] Анализ завершен. Аномалий не обнаружено.";
    }
public:
    std::string processHiddenPayload(const std::string& encryptedInput) override {
        std::string decrypted = CryptoProvider::obfuscate(encryptedInput);
        std::string aiResult = executeInternalAI(decrypted);
        return CryptoProvider::obfuscate(aiResult);
    }
};
class SenorEditorEngine {
private:
    std::unique_ptr<ISenorBlackBox> blackBox;
    SenorTaskBot taskBot;
public:
    SenorEditorEngine() {
        blackBox = std::make_unique<SenorBlackBoxImpl>();
    }
    uint64_t postEditorAction(const std::string& actionType, const std::string& codeOrText) {
        std::cout << "[Редактор] Запрос на действие '" << actionType << "' передан Task-боту.\n";
        return taskBot.assignTask(actionType, codeOrText);
    }
    void orchestrate() {
        while (taskBot.hasTasks()) {
            auto task = taskBot.getNextTask();
            if (!task) break;
            // Шаг 1: Бот готовит данные и шифрует их перед отправкой в Черный Ящик
            std::string securePayload = CryptoProvider::obfuscate(task->type + ":" + task->payload);
            // Шаг 2: Передача в Черный Ящик (внешний наблюдатель не видит логику внутри)
            std::string encryptedResult = blackBox->processHiddenPayload(securePayload);
            // Шаг 3: Расшифровка ответа Ботом на выходе из Ящика. 
            std::string finalResult = CryptoProvider::obfuscate(encryptedResult);
            // Шаг 4: Регистрация результата taskBot.updateTaskStatus(task->id, finalResult);
        }
    }
    void checkStatus(uint64_t taskId);
};