#pragma warning(push, 0)
#include <gtest/gtest.h>
#pragma warning(pop)

#include <UpdateCheckerBase.h>
#include <queue>

constexpr char MOCK_RESPONSE_ALL_RELEASES[] = R"DELIMITER([
  {
    "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408",
    "assets_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/assets",
    "upload_url": "https://uploads.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/assets{?name,label}",
    "html_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/tag/v2.5.2",
    "id": 45627408,
    "author": {
      "login": "knoxfighter",
      "id": 18680546,
      "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
      "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
      "gravatar_id": "",
      "url": "https://api.github.com/users/knoxfighter",
      "html_url": "https://github.com/knoxfighter",
      "followers_url": "https://api.github.com/users/knoxfighter/followers",
      "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
      "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
      "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
      "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
      "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
      "repos_url": "https://api.github.com/users/knoxfighter/repos",
      "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
      "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
      "type": "User",
      "site_admin": false
    },
    "node_id": "MDc6UmVsZWFzZTQ1NjI3NDA4",
    "tag_name": "v2.5.2",
    "target_commitish": "master",
    "name": "v2.5.2",
    "draft": false,
    "prerelease": false,
    "created_at": "2021-07-02T14:23:47Z",
    "published_at": "2021-07-02T14:29:54Z",
    "assets": [
      {
        "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/assets/39626428",
        "id": 39626428,
        "node_id": "MDEyOlJlbGVhc2VBc3NldDM5NjI2NDI4",
        "name": "d3d9_arcdps_killproof_me.dll",
        "label": null,
        "uploader": {
          "login": "knoxfighter",
          "id": 18680546,
          "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
          "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
          "gravatar_id": "",
          "url": "https://api.github.com/users/knoxfighter",
          "html_url": "https://github.com/knoxfighter",
          "followers_url": "https://api.github.com/users/knoxfighter/followers",
          "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
          "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
          "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
          "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
          "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
          "repos_url": "https://api.github.com/users/knoxfighter/repos",
          "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
          "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
          "type": "User",
          "site_admin": false
        },
        "content_type": "application/x-msdownload",
        "state": "uploaded",
        "size": 1437184,
        "download_count": 10343,
        "created_at": "2021-07-02T14:27:36Z",
        "updated_at": "2021-07-02T14:27:40Z",
        "browser_download_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.2/d3d9_arcdps_killproof_me.dll"
      }
    ],
    "tarball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/tarball/v2.5.2",
    "zipball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/zipball/v2.5.2",
    "body": "**Make sure you have the newest C++ 2015-2019 redistributable installed:\r\n[Microsoft download page](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0)**\r\n**[Direct download Link](https://aka.ms/vs/16/release/vc_redist.x64.exe)**\r\n\r\n- fixed coffers counted twice for LI/LD",
    "reactions": {
      "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/reactions",
      "total_count": 1,
      "+1": 0,
      "-1": 0,
      "laugh": 0,
      "hooray": 0,
      "confused": 0,
      "heart": 0,
      "rocket": 1,
      "eyes": 0
    }
  },
  {
    "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45369082",
    "assets_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45369082/assets",
    "upload_url": "https://uploads.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45369082/assets{?name,label}",
    "html_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/tag/v2.5.1",
    "id": 45369082,
    "author": {
      "login": "knoxfighter",
      "id": 18680546,
      "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
      "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
      "gravatar_id": "",
      "url": "https://api.github.com/users/knoxfighter",
      "html_url": "https://github.com/knoxfighter",
      "followers_url": "https://api.github.com/users/knoxfighter/followers",
      "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
      "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
      "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
      "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
      "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
      "repos_url": "https://api.github.com/users/knoxfighter/repos",
      "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
      "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
      "type": "User",
      "site_admin": false
    },
    "node_id": "MDc6UmVsZWFzZTQ1MzY5MDgy",
    "tag_name": "v2.5.1",
    "target_commitish": "master",
    "name": "v2.5.1",
    "draft": false,
    "prerelease": false,
    "created_at": "2021-06-28T16:18:31Z",
    "published_at": "2021-06-28T16:20:37Z",
    "assets": [
      {
        "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/assets/39365466",
        "id": 39365466,
        "node_id": "MDEyOlJlbGVhc2VBc3NldDM5MzY1NDY2",
        "name": "d3d9_arcdps_killproof_me.dll",
        "label": null,
        "uploader": {
          "login": "knoxfighter",
          "id": 18680546,
          "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
          "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
          "gravatar_id": "",
          "url": "https://api.github.com/users/knoxfighter",
          "html_url": "https://github.com/knoxfighter",
          "followers_url": "https://api.github.com/users/knoxfighter/followers",
          "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
          "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
          "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
          "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
          "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
          "repos_url": "https://api.github.com/users/knoxfighter/repos",
          "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
          "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
          "type": "User",
          "site_admin": false
        },
        "content_type": "application/x-msdownload",
        "state": "uploaded",
        "size": 1437184,
        "download_count": 1919,
        "created_at": "2021-06-28T16:20:14Z",
        "updated_at": "2021-06-28T16:20:18Z",
        "browser_download_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.1/d3d9_arcdps_killproof_me.dll"
      }
    ],
    "tarball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/tarball/v2.5.1",
    "zipball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/zipball/v2.5.1",
    "body": "**Make sure you have the newest C++ 2015-2019 redistributable installed:\r\n[Microsoft download page](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0)**\r\n**[Direct download Link](https://aka.ms/vs/16/release/vc_redist.x64.exe)**\r\n\r\n- fixed broken automatic update download\r\n- sorting respects coffers\r\n- fixed players shown twice after adding them twice\r\n- fixed clear button not clearing already tracked and manually readded players"
  }
])DELIMITER";

constexpr char MOCK_RESPONSE_LATEST_RELEASE[] = R"DELIMITER({
  "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408",
  "assets_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/assets",
  "upload_url": "https://uploads.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/assets{?name,label}",
  "html_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/tag/v2.5.2",
  "id": 45627408,
  "author": {
    "login": "knoxfighter",
    "id": 18680546,
    "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
    "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
    "gravatar_id": "",
    "url": "https://api.github.com/users/knoxfighter",
    "html_url": "https://github.com/knoxfighter",
    "followers_url": "https://api.github.com/users/knoxfighter/followers",
    "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
    "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
    "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
    "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
    "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
    "repos_url": "https://api.github.com/users/knoxfighter/repos",
    "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
    "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
    "type": "User",
    "site_admin": false
  },
  "node_id": "MDc6UmVsZWFzZTQ1NjI3NDA4",
  "tag_name": "v2.5.2",
  "target_commitish": "master",
  "name": "v2.5.2",
  "draft": false,
  "prerelease": false,
  "created_at": "2021-07-02T14:23:47Z",
  "published_at": "2021-07-02T14:29:54Z",
  "assets": [
    {
      "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/assets/39626428",
      "id": 39626428,
      "node_id": "MDEyOlJlbGVhc2VBc3NldDM5NjI2NDI4",
      "name": "d3d9_arcdps_killproof_me.dll",
      "label": null,
      "uploader": {
        "login": "knoxfighter",
        "id": 18680546,
        "node_id": "MDQ6VXNlcjE4NjgwNTQ2",
        "avatar_url": "https://avatars.githubusercontent.com/u/18680546?v=4",
        "gravatar_id": "",
        "url": "https://api.github.com/users/knoxfighter",
        "html_url": "https://github.com/knoxfighter",
        "followers_url": "https://api.github.com/users/knoxfighter/followers",
        "following_url": "https://api.github.com/users/knoxfighter/following{/other_user}",
        "gists_url": "https://api.github.com/users/knoxfighter/gists{/gist_id}",
        "starred_url": "https://api.github.com/users/knoxfighter/starred{/owner}{/repo}",
        "subscriptions_url": "https://api.github.com/users/knoxfighter/subscriptions",
        "organizations_url": "https://api.github.com/users/knoxfighter/orgs",
        "repos_url": "https://api.github.com/users/knoxfighter/repos",
        "events_url": "https://api.github.com/users/knoxfighter/events{/privacy}",
        "received_events_url": "https://api.github.com/users/knoxfighter/received_events",
        "type": "User",
        "site_admin": false
      },
      "content_type": "application/x-msdownload",
      "state": "uploaded",
      "size": 1437184,
      "download_count": 10344,
      "created_at": "2021-07-02T14:27:36Z",
      "updated_at": "2021-07-02T14:27:40Z",
      "browser_download_url": "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.2/d3d9_arcdps_killproof_me.dll"
    }
  ],
  "tarball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/tarball/v2.5.2",
  "zipball_url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/zipball/v2.5.2",
  "body": "**Make sure you have the newest C++ 2015-2019 redistributable installed:\r\n[Microsoft download page](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0)**\r\n**[Direct download Link](https://aka.ms/vs/16/release/vc_redist.x64.exe)**\r\n\r\n- fixed coffers counted twice for LI/LD",
  "reactions": {
    "url": "https://api.github.com/repos/knoxfighter/arcdps-killproof.me-plugin/releases/45627408/reactions",
    "total_count": 1,
    "+1": 0,
    "-1": 0,
    "laugh": 0,
    "hooray": 0,
    "confused": 0,
    "heart": 0,
    "rocket": 1,
    "eyes": 0
  }
}
)DELIMITER";

class UpdateCheckerMock : public UpdateCheckerBase
{
public:
	void Draw() override
	{
	}

	Status GetStatus() const
	{
		return update_status.load();
	}

	Version GetNewVersion() const
	{
		return newVersion;
	}

	std::string GetDownloadUrl() const
	{
		return downloadUrl;
	}

    std::queue<std::string> QueuedResponses;
    std::optional<std::string> HttpGet(const std::string&) override
	{
		assert(QueuedResponses.size() > 0);

        std::optional<std::string> result = QueuedResponses.front();
        QueuedResponses.pop();
        return result;
	}
};

TEST(UpdateCheckerTest, ParseVersion)
{
	UpdateCheckerMock updater;
	// "Good path"
	EXPECT_EQ(updater.ParseVersion("1.2.3"), UpdateCheckerBase::Version({1, 2, 3, 0}));
	EXPECT_EQ(updater.ParseVersion("v1.2.3"), UpdateCheckerBase::Version({1, 2, 3, 0}));
	EXPECT_EQ(updater.ParseVersion("1v.2.3"), UpdateCheckerBase::Version({1, 2, 3, 0}));
	EXPECT_EQ(updater.ParseVersion("rc1v.r2c.hh3_"), UpdateCheckerBase::Version({1, 2, 3, 0}));

	// Tokens after the third one should be ignored
	EXPECT_EQ(updater.ParseVersion("v1.2.rc3.4"), UpdateCheckerBase::Version({1, 2, 3, 0}));
	EXPECT_EQ(updater.ParseVersion("v1.2.rc3.4.5.6"), UpdateCheckerBase::Version({1, 2, 3, 0}));

	// tokens that contain no digits should be ignored
	EXPECT_EQ(updater.ParseVersion("5.4.a.3.4"), UpdateCheckerBase::Version({5, 4, 3, 0}));

	// Invalid strings should return all zero version
	EXPECT_EQ(updater.ParseVersion("1.2"), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.ParseVersion("v"), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.ParseVersion(".."), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.ParseVersion("..1"), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.ParseVersion("a.b"), UpdateCheckerBase::Version({0, 0, 0, 0}));
}

TEST(UpdateCheckerTest, IsNewer)
{
	UpdateCheckerMock updater;
	// Last version token is ignored
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 3, 0}), UpdateCheckerBase::Version({1, 2, 3, 0})));
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 3, 0}), UpdateCheckerBase::Version({1, 2, 3, 200})));
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 3, 200}), UpdateCheckerBase::Version({1, 2, 3, 0})));

	// Newer version can be detected
	EXPECT_TRUE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 4, 0}), UpdateCheckerBase::Version({1, 2, 3, 0})));
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 3, 0}), UpdateCheckerBase::Version({1, 2, 4, 0})));

	// Numbers earlier in the version number take precedence
	EXPECT_TRUE(updater.IsNewer(UpdateCheckerBase::Version({1, 3, 1, 0}), UpdateCheckerBase::Version({1, 2, 4, 0})));
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({1, 2, 4, 0}), UpdateCheckerBase::Version({1, 3, 1, 0})));
	EXPECT_TRUE(updater.IsNewer(UpdateCheckerBase::Version({10, 0, 0, 0}), UpdateCheckerBase::Version({9, 9, 9, 9})));
	EXPECT_FALSE(updater.IsNewer(UpdateCheckerBase::Version({9, 9, 9, 9}), UpdateCheckerBase::Version({10, 0, 0, 0})));
}

TEST(UpdateCheckerTest, CheckForUpdate_Stable)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push(MOCK_RESPONSE_LATEST_RELEASE);
	updater.CheckForUpdate(UpdateCheckerBase::Version({2, 5, 1, 0}), "knoxfighter/arcdps-killproof.me-plugin", false);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::UpdateAvailable);
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({2, 5, 2, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.2/d3d9_arcdps_killproof_me.dll");
}

// Same as above except the currentVersion is newer and thus an update is not available
TEST(UpdateCheckerTest, CheckForUpdate_Stable_Negative)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push(MOCK_RESPONSE_LATEST_RELEASE);
	updater.CheckForUpdate(UpdateCheckerBase::Version({3, 0, 0, 0}), "knoxfighter/arcdps-killproof.me-plugin", false);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::Unknown);

	// These are still filled out, and the status is what indicates that there is no available release
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({2, 5, 2, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.2/d3d9_arcdps_killproof_me.dll");
}

TEST(UpdateCheckerTest, CheckForUpdate_Stable_BadJson)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push("[ not json data }");
	updater.CheckForUpdate(UpdateCheckerBase::Version({0, 0, 1, 0}), "Krappa322/arcdps_unofficial_extras_releases", false);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::Unknown);
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "");
}

TEST(UpdateCheckerTest, CheckForUpdate_Prerelease)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push(MOCK_RESPONSE_ALL_RELEASES);
	updater.CheckForUpdate(UpdateCheckerBase::Version({2, 5, 1, 0}), "knoxfighter/arcdps-killproof.me-plugin", true);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::UpdateAvailable);
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({2, 5, 2, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "https://github.com/knoxfighter/arcdps-killproof.me-plugin/releases/download/v2.5.2/d3d9_arcdps_killproof_me.dll");
}

TEST(UpdateCheckerTest, CheckForUpdate_Prerelease_NoReleases)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push("[]");
	updater.CheckForUpdate(UpdateCheckerBase::Version({0, 0, 1, 0}), "Krappa322/arcdps_unofficial_extras_releases", true);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::Unknown);
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "");
}

TEST(UpdateCheckerTest, CheckForUpdate_Prerelease_BadJson)
{
	UpdateCheckerMock updater;
	updater.QueuedResponses.push("{invalidjson");
	updater.CheckForUpdate(UpdateCheckerBase::Version({0, 0, 1, 0}), "Krappa322/arcdps_unofficial_extras_releases", true);
	Sleep(1000); // Wait for spawned thread to exit
	EXPECT_EQ(updater.GetStatus(), UpdateCheckerBase::Status::Unknown);
	EXPECT_EQ(updater.GetNewVersion(), UpdateCheckerBase::Version({0, 0, 0, 0}));
	EXPECT_EQ(updater.GetDownloadUrl(), "");
}