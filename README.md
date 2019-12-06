# OS_synchronization_homework（睡覺的助教）

* git clone https://github.com/kimlin20011/OS_synchronization_homework.git
* 直接在終端機輸入 `make `


## OS environment
* macOS
* linux

## 解決方法敘述
為求每位有接受教學的學生在實驗室內外的平均時間

每個學生在自己進去實驗室的被助教教學的2單位時間，每1單位時間去確認一次外面在等待的學生人數(`numberOfOccupiedChairs`)

=> `waitingTime = waitingTime + numberOfOccupiedChairs`
來算出每位學生在實驗室外等待的時間  

最後將`waitingTime`除以`waitingStudents`(進實驗室的學生)之後再加上2（學生自己在實驗室內的時間）得到平均時間

## 預期結果

![](https://i.imgur.com/LrwUMP8.png)

## problem
* 寫MAKEFILE的時候，解決linux 無法link到外部模組 https://www.ptt.cc/bbs/C_and_CPP/M.1371650582.A.068.html https://stackoverflow.com/questions/10409032/why-am-i-getting-undefined-reference-to-sqrt-error-even-though-i-include-math
