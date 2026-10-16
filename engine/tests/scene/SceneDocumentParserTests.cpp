#include <filesystem>

#include <gtest/gtest.h>

#include "engine/scene/SceneDocumentParser.h"

namespace {
TEST(SceneDocumentParserTests, LoadsDebugSceneDocument) {
    const auto path = std::filesystem::path(__FILE__)
                          .parent_path()
                          .parent_path()
                          .parent_path() /
                      "assets/scene/debug.json";
    const auto scene = Scene::CSceneDocumentParser::Load(path);

    ASSERT_EQ(scene.name, "debug");
    ASSERT_EQ(scene.version, 1);
    ASSERT_EQ(scene.entities.size(), 8);
    EXPECT_EQ(scene.entities[1].hierarchy, "Group");

    EXPECT_EQ(
        scene.entities[2].components.at("EnemyAI").at("patrolPoints").size(),
        3);
}

TEST(SceneDocumentParserTests, ReturnsEmptyDocumentForUnknownParent) {
    const nlohmann::json data = {
        {"sceneName", "invalid"},
        {"version", 1},
        {"entities", {{{"name", "Child"}, {"parent", "Missing"}}}},
    };

    const auto scene = Scene::CSceneDocumentParser::Parse(data);
    EXPECT_TRUE(scene.name.empty());
    EXPECT_TRUE(scene.entities.empty());
}

TEST(SceneDocumentParserTests, ReturnsEmptyDocumentForWrongVectorSize) {
    const nlohmann::json data = {
        {"sceneName", "invalid"},
        {"version", 1},
        {"entities", {{{"name", "Entity"}, {"transform", {{"pos", {1, 2}}}}}}},
    };

    const auto scene = Scene::CSceneDocumentParser::Parse(data);
    EXPECT_TRUE(scene.name.empty());
    EXPECT_TRUE(scene.entities.empty());
}
} // namespace
