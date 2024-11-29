package org.nshchapps.firstyear.utils;

import android.content.Intent;
import android.content.ClipData;
import android.net.Uri;
import java.util.ArrayList;
import java.util.List;

 public class Converter  {

     public static String[] UriListToStringArray(Intent data) {
                 List<String> files = new ArrayList<>();

                 ClipData mClipData = data.getClipData();
                                int cout = data.getClipData().getItemCount();
                                for (int i = 0; i < cout; i++) {
                                    // adding imageuri in array
                                    Uri imageurl = data.getClipData().getItemAt(i).getUri();
                                    files.add(imageurl.toString());
                                }

        return files.toArray(new String[files.size()]);

    }
 }

