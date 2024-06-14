package com.go.openfrida;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.go.openfrida.core.followFrida;
import com.go.openfrida.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'openfrida' library on application startup.
    static {
        System.loadLibrary("openfrida");
    }
    String TAG  = "openFrida";

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        followFrida ff = new followFrida();
        boolean checkPort =  ff.followFridaByPort();
        if (checkPort){
            Log.i(TAG,"找到Frida by port");
        }
    }

    public native String stringFromJNI();
}














