package com.wardrums.pokeballoon;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;

public class WDNotificationService extends Service {

	private static final String TAG = "WDNotificationService";

	// 通知的静态ID
	private static int STATIC_NOTI_ID = 0;

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();

		// 启动线程去判断是否需要弹出提示
		new Thread(new Runnable() {

			@SuppressLint("WorldWriteableFiles")
			@Override
			public void run() {
				// TODO Auto-generated method stub
				try {
					while (true) {
						// 5秒检测一次
						Thread.sleep(5000);

						// 得到当前运行的上下文对象
						Context context = getApplicationContext();

						// 得到公共数据对象
						SharedPreferences sharedPreferences = context
								.getSharedPreferences("notification",
										MODE_WORLD_WRITEABLE);

						if (sharedPreferences == null)
							continue;

						// 判断下是否时间到期了，如果到期就弹出提示，并且更新当前时间
						String strNotificationText = sharedPreferences
								.getString("notificationText", "");
						if (strNotificationText.isEmpty())
							continue;

						long lTimeInterval = sharedPreferences.getLong(
								"timeInterval", 0);

						// 提示间隔默认一周时间
						if (lTimeInterval == 0)
							lTimeInterval = 86400 * 7;

						// 当前时间
						long timeNow = System.currentTimeMillis();

						// 显示通知的时间
						long lNotificationShowTime = sharedPreferences.getLong(
								"showTime", 0);
						if (lNotificationShowTime + lTimeInterval < timeNow) {
							// 更新通知弹出时间到下一个周期时间
							sharedPreferences.edit().putLong("showTime",
									timeNow + lTimeInterval);

							sharedPreferences.edit().commit();
						} else {
							// 继续下个循环判断
							continue;
						}

						// 判断数据中是否有对应到期的通知提醒
						Intent intent = new Intent(context, PokeBalloon.class);

						PendingIntent pi = PendingIntent.getActivity(context,
								0, intent, 0);

						// 生成一个通知对象
						Notification noti = new Notification.Builder(context)
								.setAutoCancel(true)
								.setContentText(strNotificationText)
								.setContentTitle(
										context.getString(R.string.app_name))
								.setTicker(strNotificationText)
								.setSmallIcon(R.drawable.icon).setWhen(timeNow)
								.setNumber(1).setContentIntent(pi).build();

						// 获取通知管理器
						NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

						// 抛出通知
						nm.notify(++STATIC_NOTI_ID, noti);
					}
				} catch (Exception e) {
					// TODO: handle exception
					e.printStackTrace();
				}
			}
		}).start();
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();

	}

	@SuppressWarnings("deprecation")
	@Override
	public void onStart(Intent intent, int startId) {
		// TODO Auto-generated method stub
		super.onStart(intent, startId);
	}
}
