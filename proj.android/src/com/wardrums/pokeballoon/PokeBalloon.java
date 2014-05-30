/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.wardrums.pokeballoon;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

import com.easyndk.classes.AndroidNDKHelper;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.umeng.socialize.bean.SHARE_MEDIA;
import com.umeng.socialize.common.SocializeConstants;
import com.umeng.socialize.controller.RequestType;
import com.umeng.socialize.controller.UMServiceFactory;
import com.umeng.socialize.controller.UMSocialService;
import com.umeng.socialize.media.UMImage;

public class PokeBalloon extends Cocos2dxActivity {

	// Handler处理类型，显示广告条和隐藏广告条
	private final static int kMessageShowAdMobView = 0;
	private final static int kMessageHiddenAdMobView = 1;

	// 显示广告的view
	private AdView mAdView = null;

	// 容纳广告条的布局器
	private LinearLayout mAdLayout = null;

	// 处理显示和隐藏广告条的消息
	private Handler mAdHandler = null;

	// 友盟社会化分享控制器
	final UMSocialService mUMSocialController = UMServiceFactory
			.getUMSocialService("com.umeng.share", RequestType.SOCIAL);

	// 当前实例，供友盟调用
	public static Activity mStaticActivity = null;

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// 设置easyNDK
		AndroidNDKHelper.SetNDKReciever(this);

		// init ad view
		mAdLayout = new LinearLayout(this);

		addContentView(mAdLayout, new LayoutParams(LayoutParams.MATCH_PARENT,
				LayoutParams.MATCH_PARENT));

		mAdHandler = new Handler() {

			@Override
			public void handleMessage(Message msg) {
				// TODO Auto-generated method stub
				super.handleMessage(msg);
				switch (msg.what) {
				case kMessageHiddenAdMobView:
					if (View.VISIBLE == mAdView.getVisibility()) {
						mAdView.clearAnimation();
						mAdView.setVisibility(View.GONE);
					}
					break;
				case kMessageShowAdMobView:
					if (View.VISIBLE != mAdView.getVisibility())
						mAdView.setVisibility(View.VISIBLE);
					break;
				default:
					break;
				}
			}

		};

		mStaticActivity = this;
	}

	public Cocos2dxGLSurfaceView onCreateView() {
		Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
		// PokeBalloon should create stencil buffer
		glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);

		return glSurfaceView;
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		if (mAdView != null)
			mAdView.resume();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		if (mAdView != null)
			mAdView.pause();
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		if (mAdView != null)
			mAdView.destroy();

		super.onDestroy();
	}

	static {
		System.loadLibrary("bspatch");
		System.loadLibrary("cocos2dcpp");
	}

	/**
	 * @return
	 */
	public static Context getContext() {
		return mStaticActivity;
	}

	/**
	 * JNI init AdView
	 * 
	 * @param strUnitID
	 *            AD unit ID
	 */
	public void initAdView(final String strUnitID) {

		this.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				if (mAdView == null) {
					// create new view
					mAdView = new AdView(mStaticActivity);

					// set Unit ID
					mAdView.setAdUnitId(strUnitID);

					// set size
					mAdView.setAdSize(AdSize.SMART_BANNER);

					// LinearLayout
					if (mAdLayout != null) {

						mAdLayout.addView(mAdView);

						// 测试机器的UDID
						AdRequest adRequest = new AdRequest.Builder()
						// .addTestDevice(
						// "F5471F6D983030F23BA3A3AD14941865")
								.build();

						// load AD
						mAdView.loadAd(adRequest);
					}
				}
			}
		});
	}

	/**
	 * JNI 设置广告条显示与隐藏
	 * 
	 * @param bShow
	 *            true 显示 false 隐藏
	 */
	public void setAdViewVisible(final boolean bShow) {
		this.runOnGLThread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				if (mAdView != null) {
					Message msg = new Message();
					if (bShow) {
						msg.what = kMessageShowAdMobView;
					} else {
						msg.what = kMessageHiddenAdMobView;
					}

					mAdHandler.sendMessage(msg);
				}
			}
		});
	}

	/**
	 * 设置友盟社会化分享的AppKey
	 * 
	 * @param strAppKey
	 */
	public void setUMengSocialAppKey(final String strAppKey) {
		SocializeConstants.APPKEY = strAppKey;

		mUMSocialController.getConfig().removePlatform(SHARE_MEDIA.RENREN,
				SHARE_MEDIA.QZONE, SHARE_MEDIA.DOUBAN);
		// mUMSocialController.getConfig().setPlatforms(SHARE_MEDIA.SINA,
		// SHARE_MEDIA.SMS, SHARE_MEDIA.FACEBOOK, SHARE_MEDIA.TWITTER);
	}

	/**
	 * 显示社交分享面板
	 * 
	 * @param strShareContent
	 *            分享内容
	 * @param strImagePath
	 *            分享图片
	 */
	public void showSNSBoard(final String strShareContent,
			final String strImagePath) {

		this.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				// 设置分享内容
				mUMSocialController.setShareContent(strShareContent);

				if (strImagePath.length() > 0) { // 设置分享图片
					mUMSocialController.setShareMedia(new UMImage(
							mStaticActivity, strImagePath));
				}

				// 打开分享面板
				mUMSocialController.openShare(mStaticActivity, false);
			}
		});
	}
}
