/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright © 2017-2019 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 */

#include <jni.h>
#include <stdlib.h>
#include <string.h>

struct go_string { const char *str; long n; };
extern int wgTurnOn(struct go_string ifname, int tun_fd, struct go_string settings);
extern void wgTurnOff(int handle);
extern int wgGetSocketV4(int handle);
extern int wgGetSocketV6(int handle);
extern char *wgGetConfig(int handle);
extern char *wgVersion();
extern void cryptoCurve25519Keygen(unsigned char pubKey[32], unsigned char privateKey[32]);

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgTurnOn(JNIEnv *env, jclass c, jstring ifname, jint tun_fd, jstring settings)
{
	const char *ifname_str = (*env)->GetStringUTFChars(env, ifname, 0);
	size_t ifname_len = (*env)->GetStringUTFLength(env, ifname);
	const char *settings_str = (*env)->GetStringUTFChars(env, settings, 0);
	size_t settings_len = (*env)->GetStringUTFLength(env, settings);
	int ret = wgTurnOn((struct go_string){
		.str = ifname_str,
		.n = ifname_len
	}, tun_fd, (struct go_string){
		.str = settings_str,
		.n = settings_len
	});
	(*env)->ReleaseStringUTFChars(env, ifname, ifname_str);
	(*env)->ReleaseStringUTFChars(env, settings, settings_str);
	return ret;
}

JNIEXPORT void JNICALL Java_com_wireguard_android_backend_GoBackend_wgTurnOff(JNIEnv *env, jclass c, jint handle)
{
	wgTurnOff(handle);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetSocketV4(JNIEnv *env, jclass c, jint handle)
{
	return wgGetSocketV4(handle);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetSocketV6(JNIEnv *env, jclass c, jint handle)
{
	return wgGetSocketV6(handle);
}

JNIEXPORT jstring JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetConfig(JNIEnv *env, jclass c, jint handle)
{
	jstring ret;
	char *config = wgGetConfig(handle);
	if (!config)
		return NULL;
	ret = (*env)->NewStringUTF(env, config);
	free(config);
	return ret;
}

JNIEXPORT jstring JNICALL Java_com_wireguard_android_backend_GoBackend_wgVersion(JNIEnv *env, jclass c)
{
	jstring ret;
	char *version = wgVersion();
	if (!version)
		return NULL;
	ret = (*env)->NewStringUTF(env, version);
	free(version);
	return ret;
}

JNIEXPORT jbyteArray JNICALL Java_com_wireguard_crypto_Key_generate(JNIEnv *env, jclass c, jbyteArray privateKey)
{
	jbyteArray pubKey;
	jbyte *pubKeyBytes, *privateKeyBytes;

	privateKeyBytes = (*env)->GetByteArrayElements(env, privateKey, NULL);
	if (!privateKeyBytes)
		return NULL;
	pubKey = (*env)->NewByteArray(env, 32);
	if (!pubKey) {
		(*env)->ReleaseByteArrayElements(env, privateKey, privateKeyBytes, JNI_ABORT);
		return NULL;
	}
	pubKeyBytes = (*env)->GetByteArrayElements(env, pubKey, NULL);
	if (!pubKeyBytes) {
		(*env)->ReleaseByteArrayElements(env, privateKey, privateKeyBytes, JNI_ABORT);
		return NULL;
	}
	cryptoCurve25519Keygen((unsigned char *)pubKeyBytes, (unsigned char *)privateKeyBytes);
	(*env)->ReleaseByteArrayElements(env, pubKey, pubKeyBytes, 0);
	(*env)->ReleaseByteArrayElements(env, privateKey, privateKeyBytes, JNI_ABORT);
	return pubKey;
}
