/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ItemDef.cpp
 * @author SPATZ.
 * @data   2014/05/17 15:23:46
 */
//---------------------------------------------------
#include "Windows.h"
#include "ItemDef.h"
#include "AppLib/Basic/Basic.h"

/*=====================================*
 * アイテム情報テーブル (csvファイルから移動)
 *=====================================*/
static const tItemDetailInfo sItemDetailInfoTable[eITEM_KIND_MAX] = {
	// ファイル名				:レイアウト名					:表示名							:値段	:説明1												: 説明2											:説明3
	{RES_MDL_MASTERSWORD,		RES_LYT_ITEM_RING_MASTERSWORD,	"マスツァーソード",				0,		"バトルエリアで「通常攻撃ボタン」で攻撃するぞ",		"伝説の剣がまさか家の中に",						"あるなんてラッキー！☆",					},
	{RES_MDL_LONGHOOK,			RES_LYT_ITEM_RING_LONGHOOK,		"フックシュート",				400,	"装備を切り替えて「アイテム使用ボタン」で使用",		"いろんな壁にひっつけたり",						"遠くの敵に攻撃できるぞ！",					},
	{RES_MDL_MAP,				RES_LYT_ITEM_RING_NONE,			"マップ",						100,	"「マップ表示ボタン」で地図を見れるぞ！",			"今自分がいる場所や",							"移動可能な場所が分かるぞ",					},
	{RES_MDL_MONEY_5,			RES_LYT_ITEM_RING_NONE,			"5ノレピー",					5,		"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",   			},
	{RES_MDL_HEART,				RES_LYT_HEART_FULL,				"ハート",  						10,		"HPをその場で1回復させるぞ！",						"",												"",	                            			},
	{RES_MDL_PIKOHUM,			RES_LYT_ITEM_RING_PIKO_HUM,		"ピコハン",						20,		"装備を切り替えて「アイテム使用ボタン」で使用",		"相手に絶妙なつっこみを",						"いれれるぞｗ",                 			},
	{"",						RES_LYT_ITEM_RING_NONE,			"なし",							0,		"装備を切り替えて「アイテム使用ボタン」で使用",		"装備なし",										"",                             			},
	{RES_MDL_BOOMERANG,			RES_LYT_ITEM_RING_BOOMERANG,	"ブーメラング",					30,		"装備を切り替えて「アイテム使用ボタン」で使用",		"遠距離の敵を攻撃したり",						"スイッチを変えたりできるぞ",   			},
	{RES_MDL_HEART_CASE,		RES_LYT_ITEM_RING_NONE,			"ハートの結晶",					0,		"最大HPが１増加するぞ！",							"",												"",	                            			},
	{RES_MDL_BOSSKEY,			RES_LYT_ITEM_RING_NONE,			"大きな鍵",						0,		"大きな宝箱やボスに続く扉の",						"鍵を開くことができるぞ！",						"",	                            			},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ITEM_RING_PORTION,		"ポーツョン",					30,		"選択画面で選択して使用",							"HPが３回復するぞ！",							"一度使用するとなくなるよ",     			},
	{RES_MDL_MONEY_10,			RES_LYT_ITEM_RING_NONE,			"10ノレピー",					10,		"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",   			},
	{RES_MDL_MONEY_50,			RES_LYT_ITEM_RING_NONE,			"50ノレピー",					50,		"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",   			},
	{RES_MDL_MONEY_100,			RES_LYT_ITEM_RING_NONE,			"100ノレピー",					100,	"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",				},
	{RES_MDL_MONEY_1,/*仮*/		RES_LYT_ITEM_RING_NONE,			"200ノレピー",					200,	"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",				},
	{RES_MDL_MONEY_1,/*仮*/		RES_LYT_ITEM_RING_NONE,			"400ノレピー",					400,	"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",				},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その１",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「０キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その２",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「１キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その３",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「２キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その４",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「３キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その５",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「４キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの笑顔その６",				999,	"リベルニア街内に隠されている６つのうちの１つだ",	"「５キー」を押すと表情が変わるぞ！",			"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_ICON_MAP_LINO_FACE,		"リノの踊り"    ,				999,	"「６キー」を押すとリノが踊りだすぞ！",				"",                         					"",                             			},
	{RES_MDL_LIB_SHIELD,		RES_LYT_ITEM_RING_SHIELD,		"リベルニアの盾",				30,		"回避ボタンを押すとで盾を構えるぞ！",	            "通常攻撃をガードしたり、",						"カミナリや光を跳ね返すぞ！",				},
	{RES_MDL_CARD_COMMON,		RES_LYT_CARD_K,					"トランプ・K",					30,		"リベルニアトランプのKINGのカードだ",				"",												"",							    			},
	{RES_MDL_CARD_COMMON,		RES_LYT_CARD_Q,					"トランプ・Q",					30,		"リベルニアトランプのQUEENのカードだ",				"",												"",											},
	{RES_MDL_CARD_COMMON,		RES_LYT_CARD_J,					"トランプ・J",					30,		"リベルニアトランプのJACKのカードだ",				"",												"",							    			},
	{RES_MDL_CRYSTAL,			RES_LYT_ITEM_RING_NONE,			"土のクリスタル",				0,		"マルイフォースの力を封印する",						"重要な力を持つクリスタルだ",					"残りのクリスタルはあと２つ！",				},
	// ※bit数が32を超えてその代替手段としてギザールの野菜をコメントアウト！
	{RES_MDL_BOMB,				RES_LYT_ITEM_RING_BOMB,			"バクダソ",						40,		"装備を切り替えて「アイテム使用ボタン」で使用",		"近くの敵に大ダメージを与えたり",				"岩を壊したりできるぞ(最大10個持てる)",		},
	{RES_MDL_CRYSTAL,			RES_LYT_ITEM_RING_NONE,			"森のクリスタル",				0,		"マルイフォースの力を封印する",						"重要な力を持つクリスタルだ",					"残りのクリスタルはあと３つ！",				},
	{RES_MDL_ARCHERY,			RES_LYT_ITEM_RING_ARCHERY,		"☆☆ゅみ☆☆",					0,		"装備を切り替えて「アイテム使用ボタン」で使用",		"遠くの敵や的を狙う事ができるぞ",	    		"スクロールで距離の調整可(最大50本持てる)",	},
	{RES_MDL_ANGEL_WING,		RES_LYT_ITEM_RING_WING,			"ペガサスの鞄",					0,		"空中にいる間にジャンプボタンで使用可能",           "鞄からペガサスの翼がはえてふんわり跳べる！", 	"リノたんマジ天使！",						},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_PRONAMA,		"プロ生ちゃんのコスプレ",		400,	"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	// 32bit以上
	{RES_MDL_MONEY_1,			RES_LYT_ITEM_RING_NONE,			"1ノレピー",					1,		"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",   			},
	{RES_MDL_MONEY_20,			RES_LYT_ITEM_RING_NONE,			"20ノレピー",					20,		"この国のお金だ",									"アイテムを買ったり",							"F4キー使用時に必要になるぞ",   			},
	{RES_MDL_PLATE_COMMON,		RES_LYT_PLATE_ARROW,			"弓矢の矢",						10,		"弓矢の矢１０本だ",									"",					                    		"",   										},
	{RES_MDL_PLATE_COMMON,		RES_LYT_PLATE_BOMB,				"爆弾の弾",						20,		"爆弾の弾３つだ",									"",					                    		"",   										},
	{RES_MDL_BIN,				RES_LYT_ITEM_RING_BIN_EMPTY,	"空きビン",						100,	"空きビンだ",									    "ビンの中に薬を入れたり",               		"妖精を捕まえたりできるぞ",         		},
	{RES_MDL_BIN,				RES_LYT_ITEM_RING_BIN_MAGENTA,	"マゼンタの薬",					120,	"マゼンタ色の薬だ",									"HPが１０回復するぞ！",                   		"(購入するには空きビンが必要)",				},
	{RES_MDL_BIN,				RES_LYT_ITEM_RING_BIN_CYAN,		"シアンの薬",					180,	"シアン色の薬だ",									"MPが全回復、ピヨリが治るぞ！",            		"(購入するには空きビンが必要)",				},
	{RES_MDL_BIN,				RES_LYT_ITEM_RING_BIN_YELLOW,	"イエローの薬",					300,	"イエロー色の薬だ",									"HP、MPともに全回復するぞ！",              		"(購入するには空きビンが必要)",				},
	{RES_MDL_BIN,				RES_LYT_ITEM_RING_BIN_FIARY,	"妖精ティンテ",					1,		"妖精のティンテだ",									"HPが４回復するぞ！",                   		"(購入するには空きビンが必要)",				},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_DABAH,			"だばぁリノちゃんのコスプレ",	0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_WALLET_A,			"かわいいサイフ",				0,		"お金を600ノレピーまで入れる事ができるぞ！",       "カエルのがまぐちでカワイイ！",   				"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_WALLET_B,			"だっせぇサイフ",				320,	"お金を1100ノレピーまで入れる事ができるぞ！",       "ＳＰＡＴＺブランドのダサいサイフだ", 			"ダサい・・・ダサすぎるぞ！",				},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_WALLET_C,			"しぶいサイフ",					680,	"お金を9999ノレピーまで入れる事ができるぞ！",       "リベルニアの貴族が使う渋いサイフだ",   		"渋いねぇ、オタク全く渋いぜ！",				},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_SEAL_KEY,			"お守りの鍵",					0,		"リノが幼き頃からずっと首にかけている鍵だ",         "何の鍵かはリノ自身も知らないけど",				"お守りの様に昔から大事に持っている",		},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_TDA_MIKU,		"ミクのコスプレ",				0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_LAPAN,			"らぱんのコスプレ",				0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_FRES,			"フレスベルクのコスプレ",		0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_MASK,			"マスク・ザ・春原のコスプレ",	0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_MAGIC_THUNDER,		"ライトニングの魔法",			0,		"メニューから選択すると使用可能",                   "リノの前方に強力なイカヅチを放つぞ！", 		"MPが大きく減るのが注意だ！",				},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_MAGIC_FLAME,		"フレイムの魔法",				0,		"メニューから選択すると使用可能",                   "リノの周辺に灼熱の炎を放つぞ！",       		"MPが減るのと詠唱に時間がかかるぞ！",		},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_MAGIC_TORNADE,		"トルネードの魔法",				0,		"メニューから選択すると使用可能",                   "リノの周辺に鋭い竜巻を発生させるぞ！", 		"MPがある程度減るから注意が必要だ！",		},
	{RES_MDL_OCARINA,			RES_LYT_RING_OCARINA,			"思ひでのオカソナ",				0,		"装備を切り替えて「アイテム使用ボタン」で使用",     "オカリナで音色を奏でて",               		"様々な効果をもたらすぞ！",					},
	{RES_MDL_COPY_ROD,			RES_LYT_RING_COPY_ROD,			"イカヅチのロッド",				666,	"装備を切り替えて「アイテム使用ボタン」で使用",     "ロッドからイカヅチを放つぞ！",         		"MPが減るので使いすぎに注意だ！",			},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_HATOPHONE,		"ハトフォンのコスプレ",			200,	"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_BAT,			"こうもりコスプレ",				100,	"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_CHILNO,			"チルノのコスプレ",				400,	"コスプレメニューからあたいのコスプレできるぞ！",   "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_HONG_KONG,		"香港映画Pのコスプレ",			0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_LOANA,			"ロアナのコスプレ",				0,		"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_YUKARI,			"結月ゆかりのコスプレ",			400,	"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_COSPLAY_NAKANO,			"鷺宮カノのコスプレ",			400,	"コスプレメニューからコスプレできるぞ！",           "",                                     		"",											},
	{RES_MDL_HEART_PIECE,		RES_LYT_ITEM_RING_NONE,			"ハートのパーツ",				0,		"４つ集めると最大HPが１増加するぞ！",				"",												"",	                            			},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_TROPHY,			"リベルニア一武道会のトロフィ",	0,		"リベルニア一武道会の全国大会を制した者だけが",     "持つことが許させる黄金のトロフィだ！",   		"お前がナンバー１だ!!",						},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_NAVI_LIGHT,		"ナビォライト",					0,		"ナビォの得意技だ！",       						"暗闇に入るとナビォが光を放ち辺りを照らすぞ！", "",											},
	{RES_MDL_BOOK_CLOSE,		RES_LYT_RING_BOOK,				"ほむらの書",					0,		"古代の民が使った",         						"「ほむほむ言語」が、これで読めるぞ！",   		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_FLAME_RING,		"炎の指輪",						300,	"このアイテムを持っていると",       				"炎系のダメージを半減するぞ！",   				"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_CHICK_PIERCE,		"ヒヨコのピアス",				560,	"このアイテムを持っていると",       				"ピヨピヨゲージの減りを3/4にするぞ！",   		"",											},
	{RES_MDL_PLATE_COMMON,		RES_LYT_RING_FAIRY_BRACELET,	"妖精の腕輪",					480,	"このアイテムを持っていると",       				"消費MPを半減するぞ！",   						"",											},
};

/*=====================================*
 * 
 *=====================================*/
static const eItemKind sKeyitemLinkTable[eKEYITEM_MAX] = {
	eITEM_KIND_MASTERSWORD,
	eITEM_KIND_SEAL_KEY,
	eITEM_KIND_WALLET_A,
	eITEM_KIND_WALLET_B,
	eITEM_KIND_WALLET_C,
	eITEM_KIND_LIB_SHIELD,
	eITEM_KIND_ANGEL_WING,
	eITEM_KIND_TROPHY,
	eITEM_KIND_BOOK,
	eITEM_KIND_FLAME_RING,
	eITEM_KIND_CHICK_PIERCE,
	eITEM_KIND_FAIRY_BRACELET,

	eITEM_KIND_NAVI_LIGHT,
	eITEM_KIND_CARD_K,
	eITEM_KIND_CARD_Q,
	eITEM_KIND_CARD_J,

	eITEM_KIND_SMILE_0,
	eITEM_KIND_SMILE_1,
	eITEM_KIND_SMILE_2,
	eITEM_KIND_SMILE_3,
	eITEM_KIND_SMILE_4,
	eITEM_KIND_SMILE_5,
	eITEM_KIND_DANCE,
};

/*=====================================*
 *
 *=====================================*/
static const char* strSongName[eSONG_TYPE_MAX] = {
	"太陽の唄",			// eSONG_TYPE_SUN,		// 太陽の唄
	"草原の唄",			// eSONG_TYPE_WILDNESS,	// 草原の唄
	"翼の唄",			// eSONG_TYPE_WING,		// 翼の唄
	"操りの唄",			// eSONG_TYPE_HANDLING,	// 操りの唄	
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
const tItemDetailInfo * GetItemDetailInfo(BYTE itemKind)
{
	return &sItemDetailInfoTable[itemKind];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
eItemKind GetKeyitemItemKind(BYTE keyitemKind)
{
	APP_ASSERT_PRINT( (keyitemKind < eKEYITEM_MAX),
					  "Invalid keyitemKind : %d \n", keyitemKind);
	return sKeyitemLinkTable[keyitemKind];
}

//---------------------------------------------------
/**
 * プレート名を取得する
 */
//---------------------------------------------------
char* GetItemPlateName(BYTE itemKind)
{
	char* str_plate_name = NULL;

	switch (itemKind) {
	case eITEM_KIND_SMILE_0:
	case eITEM_KIND_SMILE_1:
	case eITEM_KIND_SMILE_2:
	case eITEM_KIND_SMILE_3:
	case eITEM_KIND_SMILE_4:
	case eITEM_KIND_SMILE_5:
	case eITEM_KIND_DANCE:
		str_plate_name = (char *)RES_LYT_ICON_MAP_LINO_FACE;
		break;

	case eITEM_KIND_PORTION:
		str_plate_name = (char *)RES_LYT_ITEM_RING_PORTION;
		break;
	case eITEM_KIND_COSPLAY_FRES:
		str_plate_name = (char*)RES_LYT_COSPLAY_FRES;
		break;
	case eITEM_KIND_COSPLAY_DABAH:
		str_plate_name = (char*)RES_LYT_COSPLAY_DABAH;
		break;
	case eITEM_KIND_COSPLAY_SUNOPHONE:
		str_plate_name = (char*)RES_LYT_COSPLAY_HATOPHONE;
		break;
	case eITEM_KIND_COSPLAY_MASK:
		str_plate_name = (char*)RES_LYT_COSPLAY_MASK;
		break;
	case eITEM_KIND_COSPLAY_BAT:
		str_plate_name = (char*)RES_LYT_COSPLAY_BAT;
		break;
	case eITEM_KIND_COSPLAY_TDA_MIKU:
		str_plate_name = (char*)RES_LYT_COSPLAY_TDA_MIKU;
		break;
	case eITEM_KIND_COSPLAY_PRONAMA:
		str_plate_name = (char*)RES_LYT_COSPLAY_PRONAMA;
		break;
	case eITEM_KIND_COSPLAY_LAPAN:
		str_plate_name = (char*)RES_LYT_COSPLAY_LAPAN;
		break;
	case eITEM_KIND_COSPLAY_CHILNO:
		str_plate_name = (char*)RES_LYT_COSPLAY_CHILNO;
		break;
	case eITEM_KIND_COSPLAY_HONGKONG:
		str_plate_name = (char*)RES_LYT_COSPLAY_HONG_KONG;
		break;
	case eITEM_KIND_COSPLAY_LOANA:
		str_plate_name = (char*)RES_LYT_COSPLAY_LOANA;
		break;
	case eITEM_KIND_COSPLAY_YUKARI:
		str_plate_name = (char*)RES_LYT_COSPLAY_YUKARI;
		break;
	case eITEM_KIND_COSPLAY_NAKANO:
		str_plate_name = (char*)RES_LYT_COSPLAY_NAKANO;
		break;


	case eITEM_KIND_PLATE_ARROW:
		str_plate_name = (char*)RES_LYT_PLATE_ARROW;
		break;
	case eITEM_KIND_PLATE_BOMB:
		str_plate_name = (char*)RES_LYT_PLATE_BOMB;
		break;

	case eITEM_KIND_WALLET_A:
		str_plate_name = (char *)RES_LYT_RING_WALLET_A;
		break;
	case eITEM_KIND_WALLET_B:
		str_plate_name = (char *)RES_LYT_RING_WALLET_B;
		break;
	case eITEM_KIND_WALLET_C:
		str_plate_name = (char *)RES_LYT_RING_WALLET_C;
		break;
	case eITEM_KIND_SEAL_KEY:
		str_plate_name = (char *)RES_LYT_RING_SEAL_KEY;
		break;

	case eITEM_KIND_MAGIC_THUNDER:
		str_plate_name  = (char *)RES_LYT_RING_MAGIC_THUNDER;
		break;
	case eITEM_KIND_MAGIC_FLAME:
		str_plate_name  = (char *)RES_LYT_RING_MAGIC_FLAME;
		break;
	case eITEM_KIND_MAGIC_TORNADE:
		str_plate_name  = (char *)RES_LYT_RING_MAGIC_TORNADE;
		break;

	case eITEM_KIND_FLAME_RING:
		str_plate_name  = (char *)RES_LYT_RING_FLAME_RING;
		break;
	case eITEM_KIND_CHICK_PIERCE:
		str_plate_name  = (char *)RES_LYT_RING_CHICK_PIERCE;
		break;
	case eITEM_KIND_FAIRY_BRACELET:
		str_plate_name  = (char *)RES_LYT_RING_FAIRY_BRACELET;
		break;

	default:
		break;
	};

	return str_plate_name;
}

//---------------------------------------------------
/**
 * カード名を取得する
 */
//---------------------------------------------------
char* GetItemCardName(BYTE itemKind)
{
	char* str_card_name = NULL;
	
	switch (itemKind) {
		
	case eITEM_KIND_CARD_K:
		str_card_name = (char *)RES_LYT_CARD_K;
		break;
	case eITEM_KIND_CARD_Q:
		str_card_name = (char *)RES_LYT_CARD_Q;
		break;
	case eITEM_KIND_CARD_J:
		str_card_name = (char *)RES_LYT_CARD_J;
		break;
		
	default:
		break;
	};
	
	return str_card_name;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
char* GetSongName(BYTE songType)
{
	return (char *)strSongName[songType];
}

/**** end of file ****/
