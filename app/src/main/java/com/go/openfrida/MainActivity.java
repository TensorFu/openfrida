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

        // 测试
        followFrida ff = new followFrida();
        boolean checkPort =  ff.followFridaByPort();
        if (checkPort){
            Log.i(TAG,"找到Frida by port");
        }

        boolean checkD_BUS = ff.followFridaByD_Bus();
        if (checkD_BUS){
            Log.i(TAG,"Find Frida by D_BUS");
        }

        boolean check_Maps = ff.followFridaByMaps();
        if(check_Maps){
            Log.i(TAG,"Find frida by /proc/self/Maps");
        }
    }

    public native String stringFromJNI();
}














