# NIOOON Autonomous System Integration & DroidBox Virtual Android Phone Report

**API Key**: `nio_live_0a64c09c2e7f047f04afeb0a87379d7eb499c6123830fa7fed74c6c233cc5e9b`  
**Platform**: `https://nioon.lovable.app/`  
**GitHub Repository**: `https://github.com/niooon-commits/droidbox-virtual-phone`  
**Build Status**: **SUCCESS (All Native ABIs Compiled & Verified)**  
**Persistent Single Release**: `https://github.com/niooon-commits/droidbox-virtual-phone/releases/tag/latest`  
**Direct APK Download**: `https://github.com/niooon-commits/droidbox-virtual-phone/releases/download/latest/DroidBox-VirtualPhone-Latest.apk`

---

## 1. Single Release Policy Compliance (একক রিলিজ স্ট্র্যাটেজি)
ব্যবহারকারীর নির্দিষ্ট নির্দেশনা অনুযায়ী:
> "প্রতিটি Build-এর জন্য আলাদা আলাদা Release তৈরি করা যাবে না। শুধুমাত্র একটি Release থাকবে। পরবর্তীতে যখনই নতুন APK Build হবে, তখন নতুন Release তৈরি না করে আগের সেই একই Release-এর মধ্যে থাকা APK ফাইলটি নতুন Build হওয়া APK ফাইল দিয়ে Replace/Update করে দিতে হবে।"

- **Tag Name**: `latest`
- **Asset Name**: `DroidBox-VirtualPhone-Latest.apk`
- **Release Strategy**: GitHub Actions-এর `gh release edit` এবং `gh release upload latest ... --clobber` ব্যবহার করা হয়েছে। ফলে প্রতিবার `main` ব্রাঞ্চে পুশ হলে কোনো নতুন রিলিজ তৈরি না হয়ে আগের `latest` রিলিজের ভেতরে থাকা `DroidBox-VirtualPhone-Latest.apk` ফাইলটি স্বয়ংক্রিয়ভাবে রিপ্লেস এবং আপডেট হয়ে যায়।

---

## 2. Android & C++ Virtual Phone Engine Architecture
DroidBox ভার্চুয়াল ইঞ্জিনটি VMOS, VirtualApp এবং Twoyi আর্কিটেকচার অনুযায়ী তৈরি:
1. **Linux Namespaces (`clone`, `unshare`)**: PID, Mount, Network, IPC এবং UTS আইসোলেশন।
2. **Virtual File System (VFS)**: রুট ছাড়া পৃথক `/data/user/0/<virtual_uid>` স্পেস তৈরি এবং ফেক `build.prop` ইনজেকশন।
3. **Binder IPC Hooking**: `ioctl(binder_fd, BINDER_WRITE_READ, ...)` ইন্টারসেপ্ট করে Virtual Package Manager ও Virtual Activity Manager ভার্চুয়ালাইজেশন।
4. **SurfaceFlinger Virtual Compositor**: 1080x2400 @ 60 FPS RGBA ভার্চুয়াল ফ্রেমবাফার যা হোস্ট অ্যাপ্লিকেশনের ভাসমান উইন্ডো (Floating Window) অথবা ফুলস্ক্রিন ভিউতে রেন্ডার হয়।
5. **GMS Core & Play Store Bridge**: ভার্চুয়াল Google Services Framework (GSF) ID জেনারেট করে Google Play Store ও YouTube সাপোর্ট।

---

## 3. GitHub Actions CI/CD Pipeline
`.github/workflows/android-build-release.yml`:
- JDK 17 (Temurin) পরিবেশ সেটআপ
- Android SDK লাইসেন্স অটো-অ্যাকসেপ্ট
- আরএসএ ২০৪৮-বিট অ্যান্ড্রয়েড ডিবাগ কীস্টোর জেনারেশন
- Gradle C++20 NDK কম্পাইলেশন (`arm64-v8a`, `armeabi-v7a`, `x86`, `x86_64`)
- APK অ্যাসেম্বল ও সাইনিং
- একক রিলিজ ক্লাবার আপলোড (`gh release upload --clobber`)
