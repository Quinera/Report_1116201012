#pragma once
#define BUFFSIZE 2048

struct VideoItem //動画情報の構造体
{
	String title;	//タイトル
	String publishedAt; //投稿日時
	String viewCount;	//視聴回数
	String likeCount;	//高評価数
	String commentCount;	//コメント数
};

class YoutubeData
{
public:
	YoutubeData(const String& channelID, const String& apiKey) //コンストラクタ
		: m_channelId(channelID), m_apikey(apiKey)
	{
	}

	bool outputData(const char* fileName, Array<VideoItem>& items) //出力用メソッド
	{
		int i = 0;
		FILE* fp;
		errno_t error;
		error = fopen_s(&fp, fileName, "w");	//ファイルオープン
		if (error != 0) {
			Print << U"出力ファイルの生成に失敗しました。";
			return false;
		}
		else {
			fprintf(fp, "title,publishedAt,viewCount,likeCount,commentCount\n");
			for (i = 0; i < items.size(); i++) {	//動画情報をitemsの個数分入力
				fprintf(fp, "%s,%s,%s,%s,%s\n", items[i].title.narrow().c_str(), items[i].publishedAt.narrow().c_str(), items[i].viewCount.narrow().c_str(), items[i].likeCount.narrow().c_str(), items[i].commentCount.narrow().c_str());
			}
			fclose(fp);		//ファイルクローズ
			return true;
		}
	}

	bool getNewItems(Array<VideoItem>& items)
	{

		const HashTable<String, String> headers = { { U"Content-Type", U"application/json" } };
		Array<VideoItem> res;	//結果保存用動的配列
		String result;	//HTTPのレスポンス保存用
		bool fin = false;	//終了判定
		int32 totalresult = 0;	//全動画件数
		int32 cnt = 1;	//ループカウント用

		do {
			//URL(videoID取得用)
			URL url = U"https://www.googleapis.com/youtube/v3/search?part=id&maxResults=50&type=video&channelId=" + m_channelId + U"&key=" + m_apikey;

			if (!m_nextPageToken.empty())		//nextPageTokenを指定
			{
				url += U"&pageToken=" + m_nextPageToken;
			}
			if (HTTPGet(url, headers, result))	//チャンネルのvideoID一覧を取得
			{
				JSON json = JSON::Parse(result);	//結果をjson型に格納

				totalresult = json[U"pageInfo"][U"totalResults"].get<int32>();	//videoの総件数を取得
				if (cnt * 50 < totalresult)	//結果が次ページに跨がる場合
				{
					m_nextPageToken = json[U"nextPageToken"].getString();	//nextPageTokenを取得
				}
				else {
					fin = true;		//終了
				}

				for (const auto& object : json[U"items"].arrayView()) {	//取得した動画数分

					m_videoId = object[U"id"][U"videoId"].getString();		//videoIdを取得
					//URL(動画情報取得用)
					URL url2 = U"https://www.googleapis.com/youtube/v3/videos?part=snippet,statistics&maxResults=50&key=" + m_apikey;
					url2 += U"&id=" + m_videoId;

					if (HTTPGet(url2, headers, result))	//動画情報を取得
					{
						JSON json2 = JSON::Parse(result);		//結果をjson型に格納

						VideoItem item;
						//動画の情報を取得
						item.title = json2[U"items"][0][U"snippet"][U"title"].getString();
						item.publishedAt = json2[U"items"][0][U"snippet"][U"publishedAt"].getString();
						item.viewCount = json2[U"items"][0][U"statistics"][U"viewCount"].getString();
						item.likeCount = json2[U"items"][0][U"statistics"][U"likeCount"].getString();
						item.commentCount = json2[U"items"][0][U"statistics"][U"commentCount"].getString();
						res << item;		//動的配列にプッシュ
					}
				}
			}
			else
			{
				return false;
			}
			cnt++;
			Print << U"動画情報取得中... 現在" << res.size() << U"件";
			System::Update();
		} while (!fin);	//終了判定
		items = res;	//引数に結果を反映する
		return true;
	}

private:

	bool HTTPGet(const URL& url, const HashTable<String, String>& headers, String& result) //APIへのHTTPリクエスト
	{

		MemoryWriter writer;	//バイナリデータ保存用

		if (auto response = SimpleHTTP::Get(url, headers, writer))	//API結果をwriterに保存
		{
			if (response.isOK())
			{
				auto res = writer.getBlob().asArray();	//結果を配列データに変換
				std::string s;	//
				for (auto elem : res)		//resの行数分繰り返し
				{
					s += (char)elem;	//一行ごとにchar型に変換しstd::string変数へ保存
				}
				result = Unicode::FromUTF8(s);		//結果をUTF8からStringへ変換
				return true;
			}
		}
		else
		{
			Print << U"動画情報の取得に失敗しました。";
			return false;
		}
		return false;
	}

private:
	String m_apikey;	//APIキー
	String m_videoId;	//動画ID
	String m_channelId;	//チャンネルID
	String m_nextPageToken;	//次ページトークン
};
