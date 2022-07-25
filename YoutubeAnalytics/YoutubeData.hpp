#pragma once
#define BUFFSIZE 2048

struct VideoItem //動画情報の構造体
{
	String title;	//タイトル
	String publishedAt; //投稿日時
	String viewCount;	//視聴回数
	String likeCount;	//高評価数
	String commentCount;	//コメント数
	String description;	//動画説明文
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
		error = fopen_s(&fp, fileName, "w");	//ファイルオープン
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

		URL url = U"https://www.googleapis.com/youtube/v3/search?part=id&type=video&channelId=" + m_channelId + U"&key=" + m_apikey;
		const HashTable<String, String> headers = { { U"Content-Type", U"application/json" } };

		if (!m_nextPageToken.empty())
		{
			url += U"&pageToken=" + m_nextPageToken;
		}

		String result;
		Print << url;
		if (HTTPGet(url, headers, result))	//チャンネルのvideoID一覧を取得
		{
			JSON json = JSON::Parse(result);
			if (!json[U"nextPageToken"].isEmpty()) m_nextPageToken = json[U"nextPageToken"].getString();
			Array<VideoItem> res;

			for (const auto& object : json[U"items"].arrayView()) {	//取得した動画数分

				m_videoId = object[U"id"][U"videoId"].getString();		//videoIdを取得
				URL url2 = U"https://www.googleapis.com/youtube/v3/videos?part=snippet,statistics&key=" + m_apikey;
				url2 += U"&id=" + m_videoId;
				//Print << m_videoId;

				if (HTTPGet(url2, headers, result))	//動画情報を取得
				{
					JSON json2 = JSON::Parse(result);

					VideoItem item;
					item.title = json2[U"items"][0][U"snippet"][U"title"].getString();
					item.publishedAt = json2[U"items"][0][U"snippet"][U"publishedAt"].getString();
					item.viewCount = json2[U"items"][0][U"statistics"][U"viewCount"].getString();
					item.likeCount = json2[U"items"][0][U"statistics"][U"likeCount"].getString();
					item.commentCount = json2[U"items"][0][U"statistics"][U"commentCount"].getString();
					item.description = json2[U"items"][0][U"snippet"][U"description"].getString();
					res << item;		//動的配列にプッシュ
				}
			}
			items = res;	//引数に結果を反映する
			return true;
		}
		else
		{
			return false;
		}

	}

private:

	bool HTTPGet(const URL& url, const HashTable<String, String>& headers, String& result) //APIへのHTTPリクエスト
	{

		MemoryWriter writer;	//バイナリデータ保存用

		if (auto response = SimpleHTTP::Get(url, headers, writer))	//API結果をjson形式でwriterに保存
		{
			if (response.isOK())
			{
				auto res = writer.getBlob().asArray();	//結果を文字列データに変換
				std::string s;	//
				for (auto elem : res)		//resの行数分繰り返し
				{
					s += (char)elem;	//一行ごとにstd::string変数へ保存
				}
				result = Unicode::FromUTF8(s);		//結果をUTF8からStringへ変換
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
