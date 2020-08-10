package com.example.jniexample;

import android.os.Bundle;
import android.widget.TextView;

import androidx.annotation.Keep;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
	static {
		System.loadLibrary("jni-example-lib");
	}

	private TextView mTextView;


	public native void registerCallback();

	public native void deregisterCallback();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mTextView = findViewById(R.id.textView);
	}

	@Override
	public void onResume() {
		super.onResume();
		registerCallback();
	}

	@Override
	public void onPause() {
		super.onPause();
		deregisterCallback();
	}

	@Keep
	@SuppressWarnings("unused")
	private void onCallback() {
		// Background views can not touch views created by original thread
		// that created the view hierarchy.
		// Else it will throw `CalledFromWrongThreadException`

		// Alternatively, better way would be to start threads from java itself and call jni from
		// there. As suggested in android docs, this approach correctly sets threadName and
		// threadGroup
		runOnUiThread(() -> MainActivity.this.mTextView.setText(R.string.callback_success));
	}

}
