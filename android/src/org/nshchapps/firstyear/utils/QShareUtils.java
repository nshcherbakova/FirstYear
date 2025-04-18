/*!
 * MIT License
 *
 * Copyright (c) 2019 Tim Seemann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// (c) 2017 Ekkehard Gentz (ekke)
// this project is based on ideas from
// http://blog.lasconic.com/share-on-ios-and-android-using-qml/
// see github project https://github.com/lasconic/ShareUtils-QML
// also inspired by:
// https://www.androidcode.ninja/android-share-intent-example/
// https://www.calligra.org/blogs/sharing-with-qt-on-android/
// https://stackoverflow.com/questions/7156932/open-file-in-another-app
// http://www.qtcentre.org/threads/58668-How-to-use-QAndroidJniObject-for-intent-setData
// https://stackoverflow.com/questions/5734678/custom-filtering-of-intent-chooser-based-on-installed-android-package-name
// see also /COPYRIGHT and /LICENSE

package org.nshchapps.firstyear.utils;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Parcelable;
import android.provider.MediaStore;
import androidx.core.app.ShareCompat;
import android.app.Activity;

import androidx.core.content.FileProvider;
import android.util.Log;

import org.qtproject.qt.android.QtNative;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.String;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class QShareUtils {
    // reference Authority as defined in AndroidManifest.xml
    private static String AUTHORITY = "org.nshchapps.firstyear.shareutils.fileprovider";
    private static String TAG = "QShareUtils";

    private static Activity m_activity = null;

    public static Activity getActivity() {
        return QtNative.activity();
    //    return m_activity;
    }

    public static void setActivity(Activity activity) {
        m_activity = activity;
        return;
    }

    protected QShareUtils() {
        // Log.d(TAG, "QShareUtils()");
    }

    public static boolean checkMimeTypeView(String mimeType) {
        if (getActivity() == null) {
            return false;
        }
        Intent myIntent = new Intent();
        myIntent.setAction(Intent.ACTION_VIEW);
        // without an URI resolve always fails
        // an empty URI allows to resolve the Activity
        File fileToShare = new File("");
        Uri uri = Uri.fromFile(fileToShare);
        myIntent.setDataAndType(uri, mimeType);

        // Verify that the intent will resolve to an activity
        if (myIntent.resolveActivity(getActivity().getPackageManager()) != null) {
            // Log.d(TAG, "checkMime YEP - we can go on and View");
            return true;
        } else {
            // Log.d(TAG, "checkMime sorry - no App available to View");
        }
        return false;
    }

    public static boolean share(String text, String url) {
        if (getActivity() == null) {
            return false;
        }
        Intent sendIntent = new Intent();
        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_TEXT, text + " " + url);
        sendIntent.setType("text/plain");

        // Verify that the intent will resolve to an activity
        if (sendIntent.resolveActivity(getActivity().getPackageManager()) != null) {
            getActivity().startActivity(sendIntent);
            return true;
        } else {
            Log.d(TAG, "share Intent not resolved");
        }
        return false;
    }

    // thx @oxied and @pooks for the idea:
    // https://stackoverflow.com/a/18835895/135559 theIntent is already configured
    // with all needed properties and flags so we only have to add the packageName
    // of targeted app
  /*  public static boolean createCustomChooserAndStartActivity(Intent theIntent,
                                                              String title,
                                                              int requestId,
                                                              Uri uri) {
        final Context context = getActivity();
        final PackageManager packageManager = context.getPackageManager();
        final boolean isLowerOrEqualsKitKat = Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT;

        // MATCH_DEFAULT_ONLY: Resolution and querying flag. if set, only filters
        // that support the CATEGORY_DEFAULT will be considered for matching. Check
        // if there is a default app for this type of content.
        ResolveInfo defaultAppInfo =
                packageManager.resolveActivity(theIntent, PackageManager.MATCH_DEFAULT_ONLY);
        if (defaultAppInfo == null) {
            Log.d(TAG, title + " PackageManager cannot resolve Activity");
            return false;
        }

        // Retrieve all apps for our intent. Check if there are any apps returned
        List<ResolveInfo> appInfoList =
                packageManager.queryIntentActivities(theIntent, PackageManager.MATCH_DEFAULT_ONLY);
        if (appInfoList.isEmpty()) {
            Log.d(TAG, title + " appInfoList.isEmpty");
            return false;
        }
        // Log.d(TAG, title + " appInfoList: " + appInfoList.size());

        // Sort in alphabetical order
        Collections.sort(appInfoList, new Comparator<ResolveInfo>() {
            @Override
            public int compare(ResolveInfo first, ResolveInfo second) {
                String firstName = first.loadLabel(packageManager).toString();
                String secondName = second.loadLabel(packageManager).toString();
                return firstName.compareToIgnoreCase(secondName);
            }
        });

        List<Intent> targetedIntents = new ArrayList<Intent>();
        // Filter itself and create intent with the rest of the apps.
        for (ResolveInfo appInfo : appInfoList) {
            // get the target PackageName
            String targetPackageName = appInfo.activityInfo.packageName;
            // we don't want to share with our own app
            // in fact sharing with own app with resultCode will crash because doesn't
            // work well with launch mode 'singleInstance'
            if (targetPackageName.equals(context.getPackageName())) {
                continue;
            }
            // if you have a blacklist of apps please exclude them here

            // we create the targeted Intent based on our already configured Intent
            Intent targetedIntent = new Intent(theIntent);
            // now add the target packageName so this Intent will only find the one
            // specific App
            targetedIntent.setPackage(targetPackageName);
            // collect all these targetedIntents
            targetedIntents.add(targetedIntent);

            // legacy support and Workaround for Android bug
            // grantUriPermission needed for KITKAT or older
            // see https://code.google.com/p/android/issues/detail?id=76683
            // also:
            // https://stackoverflow.com/questions/18249007/how-to-use-support-fileprovider-for-sharing-content-to-other-apps
            if (isLowerOrEqualsKitKat) {
                // Log.d(TAG, "legacy support grantUriPermission");
                context.grantUriPermission(targetPackageName,
                                           uri,
                                           Intent.FLAG_GRANT_READ_URI_PERMISSION
                                                   | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
                // attention: you must revoke the permission later, so this only makes
                // sense with getting back a result to know that Intent was done I
                // always move or delete the file, so I don't revoke permission
            }
        }

        // check if there are apps found for our Intent to avoid that there was only
        // our own removed app before
        if (targetedIntents.isEmpty()) {
            Log.d(TAG, title + " targetedIntents.isEmpty");
            return false;
        }

        // now we can create our Intent with custom Chooser
        // we need all collected targetedIntents as EXTRA_INITIAL_INTENTS
        // we're using the last targetedIntent as initializing Intent, because
        // chooser adds its initializing intent to the end of EXTRA_INITIAL_INTENTS
        // :)
        Intent chooserIntent =
                Intent.createChooser(targetedIntents.remove(targetedIntents.size() - 1), title);
        if (targetedIntents.isEmpty()) {
            // Log.d(TAG, title + " only one Intent left for Chooser");
        } else {
            chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS,
                                   targetedIntents.toArray(new Parcelable[] {}));
        }
        // Verify that the intent will resolve to an activity
        if (chooserIntent.resolveActivity(getActivity().getPackageManager()) != null) {
            if (requestId > 0) {
                getActivity().startActivityForResult(chooserIntent, requestId);
            } else {
                getActivity().startActivity(chooserIntent);
            }
            return true;
        }
        return false;
    }*/

    // I am deleting the files from shared folder when Activity was done or
    // canceled so probably I don't have to revike FilePermissions for older OS if
    // you don't delete or move the file: here's what you must done to revoke the
    // access
    public static void revokeFilePermissions(String filePath) {
        final Context context = getActivity();
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT) {
            File file = new File(filePath);
            Uri uri = FileProvider.getUriForFile(context, AUTHORITY, file);
            context.revokeUriPermission(
                    uri,
                    Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        }
    }

    public static boolean sendFile(String filePath, String title, String mimeType, int requestId) {
        if (getActivity() == null) {
            return false;
        }

        // using v4 support library create the Intent from ShareCompat
        // Intent sendIntent = new Intent();
        Intent sendIntent = ShareCompat.IntentBuilder.from(getActivity()).getIntent();
        sendIntent.setAction(Intent.ACTION_SEND);

        File fileToShare = new File(filePath);

        // Using FileProvider you must get the URI from FileProvider using your
        // AUTHORITY Uri uri = Uri.fromFile(imageFileToShare);
        Uri uri;
        try {
            uri = FileProvider.getUriForFile(getActivity(), AUTHORITY, fileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("error", e.getMessage());
            Log.d(TAG, "sendFile - cannot be shared: " + filePath);
            return false;
        }

        sendIntent.putExtra(Intent.EXTRA_STREAM, uri);

        if (mimeType == null || mimeType.isEmpty()) {
            // fallback if mimeType not set
            mimeType = getActivity().getContentResolver().getType(uri);
            // Log.d(TAG, "sendFile guessed mimeType:" + mimeType);
        } else {
            // Log.d(TAG, "sendFile w mimeType:" + mimeType);
        }

        sendIntent.setType(mimeType);

        sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        sendIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        //return createCustomChooserAndStartActivity(sendIntent, title, requestId, uri);
        getActivity().startActivity(Intent.createChooser(sendIntent, null));
        return true;
    }

   /* public static boolean viewFile(String filePath, String title, String mimeType, int requestId) {
        if (getActivity() == null) {
            return false;
        }

        // using v4 support library create the Intent from ShareCompat
//         Intent viewIntent = new Intent();
        Intent viewIntent = ShareCompat.IntentBuilder.from(getActivity()).getIntent();
        viewIntent.setAction(Intent.ACTION_VIEW);

        File fileToShare = new File(filePath);

        // Using FileProvider you must get the URI from FileProvider using your
        // AUTHORITY
        Uri uri;
        try {
            uri = FileProvider.getUriForFile(getActivity(), AUTHORITY, fileToShare);
        } catch (IllegalArgumentException e) {
             Log.d(TAG, "viewFile - cannot be viewed: " + filePath, e);
            return false;
        }
        // now we got a content URI per ex
        // content://org.ekkescorner.examples.sharex.fileprovider/my_shared_files/qt-logo.png
        // from a fileUrl:
        // /data/user/0/org.ekkescorner.examples.sharex/files/share_example_x_files/qt-logo.png
        // Log.d(TAG, "viewFile from file path: " + filePath);
        // Log.d(TAG, "viewFile to content URI: " + uri.toString());

        if (mimeType == null || mimeType.isEmpty()) {
            // fallback if mimeType not set
            mimeType = getActivity().getContentResolver().getType(uri);
            // Log.d(TAG, "viewFile guessed mimeType:" + mimeType);
        } else {
            // Log.d(TAG, "viewFile w mimeType:" + mimeType);
        }

        viewIntent.setDataAndType(uri, mimeType);

        viewIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        viewIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        return createCustomChooserAndStartActivity(viewIntent, title, requestId, uri);
    }*/

  /* public static boolean editFile(String filePath, String title, String mimeType, int requestId) {
        if (getActivity() == null)
            return false;

        // using v4 support library create the Intent from ShareCompat
        // Intent editIntent = new Intent();
        Intent editIntent = ShareCompat.IntentBuilder.from(getActivity()).getIntent();
        editIntent.setAction(Intent.ACTION_EDIT);

        File imageFileToShare = new File(filePath);

        // Using FileProvider you must get the URI from FileProvider using your
        // AUTHORITY Uri uri = Uri.fromFile(imageFileToShare);
        Uri uri;
        try {
            uri = FileProvider.getUriForFile(getActivity(), AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            // Log.d(TAG, "editFile - cannot be shared: " + filePath);
            return false;
        }
        // Log.d(TAG, "editFile" + uri.toString());

        if (mimeType == null || mimeType.isEmpty()) {
            // fallback if mimeType not set
            mimeType = getActivity().getContentResolver().getType(uri);
            // Log.d(TAG, "editFile guessed mimeType:" + mimeType);
        } else {
            // Log.d(TAG, "editFile w mimeType:" + mimeType);
        }

        editIntent.setDataAndType(uri, mimeType);

        editIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        editIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        return createCustomChooserAndStartActivity(editIntent, title, requestId, uri);
    }*/

    public static String getContentName(ContentResolver cR, Uri uri) {
        Cursor cursor = cR.query(uri, null, null, null, null);
        cursor.moveToFirst();
        int nameIndex = cursor.getColumnIndex(MediaStore.MediaColumns.DISPLAY_NAME);
        if (nameIndex >= 0) {
            return cursor.getString(nameIndex);
        } else {
            return null;
        }
    }

    public static String createFile(ContentResolver cR, Uri uri, String fileLocation) {
        String filePath = null;
        try {
            InputStream iStream = cR.openInputStream(uri);
            if (iStream != null) {
                String name = getContentName(cR, uri);
                if (name != null) {
                    filePath = fileLocation + "/" + name;
                    // Log.d(TAG, "- create File" + filePath);
                    File f = new File(filePath);
                    FileOutputStream tmp = new FileOutputStream(f);
                    // Log.d(TAG, "- create File new FileOutputStream");

                    int bufferSize = 1024;
                    byte[] buffer = new byte[bufferSize];
                    ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                    while (iStream.available() > 0) {
                        int n = iStream.read(buffer);
                        // write to output stream
                        outputStream.write(buffer, 0, n);
                    }
                    // write output stream to file in one chunk
                    tmp.write(outputStream.toByteArray());
                    outputStream.close();
                    tmp.close();
                    iStream.close();
                    return filePath;
                } // name
            }     // iStream
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return filePath;
        } catch (IOException e) {
            e.printStackTrace();
            return filePath;
        } catch (Exception e) {
            e.printStackTrace();
            return filePath;
        }
        return filePath;
    }
}
