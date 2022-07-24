#pragma once
#define BUFFSIZE 2048

struct VideoItem
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
	YoutubeData(const String& channelID, const String& apiKey)
		: m_channelId(channelID), m_apikey(apiKey)
	{

	}
	~YoutubeData()
	{

	}

	bool outputData(const char* fileName, char s1[][BUFFSIZE], char s2[][BUFFSIZE], char s3[][BUFFSIZE], char s4[][BUFFSIZE], char s5[][BUFFSIZE], char s6[][BUFFSIZE], int cnt)
	{

		int i = 0;
		FILE* fp;
		errno_t error;
		error = fopen_s(&fp, fileName, "w");
		if (error != 0)
			fprintf_s(stderr, "failed to open");
		else {
			fprintf(fp, "title,viewCount,likeCount,commentCount,publishedAt\n");
			for (i = 0; i < cnt; i++) {
				fprintf(fp, "%s,%s,%s,%s,%s,%s\n", s1[i], s2[i], s3[i], s4[i], s5[i], s6[i]);
			}
			fclose(fp);
		}
	}

	bool getYoutubeData()
	{
		const URL url = U"https://www.googleapis.com/youtube/v3/search?part=id&channelId=" + m_channelId + U"&key=" + m_apikey;
		const HashTable<String, String> headers = { { U"Content-Type", U"application/json" } };

		String result;
		if (HTTPGet(url, headers, result))
		{
			JSON json = JSON::Parse(result);

			m_videoId = json[U"items"][0][U"id"][U"videoId"].getString();


			return true;
		}
		else
		{
			return false;
		}
	}

	bool getNewItems(Array<VideoItem>& items)
	{
		if (m_videoId.empty())
		{
			return false;
		}

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
