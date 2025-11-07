#include "engine/utils/FileHandler.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

class FileHandlerTest : public ::testing::Test {
protected:
    FileHandlerTest() {
        fileHandler.CreateTempFolder("FileHandlerTest");
    }
    void TearDown() override {
        fileHandler.DeleteFile(testFilename, ".json");
    }

    Utils::CFileHandler fileHandler{};
    const std::string testFilename = "test";
};

TEST_F(FileHandlerTest, HasEmptyFolder) {
    std::string tempFolder = fileHandler.GetTempFolder();
    EXPECT_FALSE(tempFolder.empty());
}

TEST_F(FileHandlerTest, SaveAndLoadJson) {
    nlohmann::json jsonData;
    jsonData["key1"] = "value1";
    jsonData["key2"] = 42;

    EXPECT_TRUE(fileHandler.SaveJson(testFilename, jsonData));

    nlohmann::json loadedData = fileHandler.LoadJson(testFilename);
    EXPECT_EQ(loadedData, jsonData);

    loadedData["key3"] = "new_value";
    EXPECT_TRUE(fileHandler.SaveJson(testFilename, loadedData));

    nlohmann::json reloadedData = fileHandler.LoadJson(testFilename);
    EXPECT_EQ(reloadedData, loadedData);
}
