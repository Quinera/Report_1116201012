#pragma once
#define BUFFSIZE 2048

struct VideoItem //動画情報の構造体
{
	String title;
	String publishedAt;
	String viewCount;
	String likeCount;
	String commentCount;
	String description;
};

class YoutubeData
{
public:
	YoutubeData(const String& channelID, const String& apiKey) //コンストラクタ
		: m_channelId(channelID), m_apikey(apiKey)
	{

	}
	~YoutubeData()
	{

	}

	bool outputData(const char* fileName, Array<VideoItem>& items) //出力用メソッド
	{
		int i = 0;
		FILE* fp;
		errno_t error;
		error = fopen_s(&fp, fileName, "w");
		if (error != 0) {
			fprintf_s(stderr, "failed to open");
			return false;
		}
		else {
			fprintf(fp, "title,publishedAt,viewCount,likeCount,commentCount,description\n");
			for (i = 0; i < items.size(); i++) {
				fprintf(fp, "%s,%s,%s,%s,%s,%s\n", items[i].title.narrow().c_str(), items[i].publishedAt.narrow().c_str(), items[i].viewCount.narrow().c_str(), items[i].likeCount.narrow().c_str(), items[i].commentCount.narrow().c_str(), items[i].description.narrow().c_str());
			}
			fclose(fp);
			return true;
		}
	}

	bool getNewItems(Array<VideoItem>& items)
	{

		URL url = U"https://www.googleapis.com/youtube/v3/search?part=id&channelId=" + m_channelId + U"&key=" + m_apikey;
		const HashTable<String, String> headers = { { U"Content-Type", U"application/json" } };

		if (!m_nextPageToken.empty())
		{
			url += U"&pageToken=" + m_nextPageToken;
		}

		String result;
		if (HTTPGet(url, headers, result))
		{
			JSON json = JSON::Parse(result);

			URL url2 = U"https://www.googleapis.com/youtube/v3/videos?part=snippet,statistics&id=" + m_videoId + U"&key=" + m_apikey;
			m_videoId = json[U"items"][0][U"id"][U"videoId"].getString();


			return true;
		}

		if (HTTPGet(url, headers, result))
		{
			JSON json = JSON::Parse(result);

			m_nextPageToken = json[U"nextPageToken"].getString();

			Array<VideoItem> res;
			for (const auto& object : json[U"items"].arrayView())
			{
				VideoItem item;
				item.title = object[U"items"][U"snippet"][U"title"].getString();
				item.publishedAt = object[U"items"][U"snippet"][U"publishedAt"].getString();
				item.viewCount = object[U"items"][U"statisitcs"][U"viewCount"].getString();
				item.likeCount = object[U"items"][U"statisitcs"][U"likeCount"].getString();
				item.commentCount = object[U"items"][U"statisitcs"][U"commentCount"].getString();
				item.description = object[U"items"][U"snippet"][U"description"].getString();
				res << item;
			}

			items = res;

			return true;
		}
		else
		{
			return false;
		}

	}

private:

	bool HTTPGet(const URL& url, const HashTable<String, String>& headers, String& result)
	{

		MemoryWriter writer;

		if (auto response = SimpleHTTP::Get(url, headers, writer))
		{
			if (response.isOK())
			{
				auto res = writer.getBlob().asArray();
				std::string s;
				for (auto elem : res)
				{
					s += (char)elem;
				}
				result = Unicode::FromUTF8(s);
				return true;
			}
		}
		else
		{
			return false;
		}

		return false;
	}

private:
	String m_apikey;
	String m_videoId;
	String m_channelId;
	String m_nextPageToken;
};
