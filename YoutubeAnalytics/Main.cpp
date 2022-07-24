# include <Siv3D.hpp> // OpenSiv3D v0.6.4
#include <windows.h>
# include "YouTubeData.hpp"
# define BUFFSIZE 2048
# define API_KEY U"AIzaSyDQjz4TL0FPXUshaMWFXMRFOgGALVhURTw"  // 取得した API キー


void Main()
{
	char chStr[BUFFSIZE];
	DWORD dwLen = 0;
	GetPrivateProfileStringA("Data", "ChannelID", "0", chStr, BUFFSIZE - 1, ".\\Setting.ini");
	String CHANNEL_ID = Unicode::Widen(chStr);
	YoutubeData ytdata(CHANNEL_ID, API_KEY);
	if (!ytdata.getYoutubeData())
	{
		return;
	}
	Array<VideoItem> items;
	ytdata.getNewItems(items);

	while (System::Update())
	{

	}
}
