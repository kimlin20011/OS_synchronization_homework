# OS_synchronization_homework

* git clone https://github.com/kimlin20011/OS_synchronization_homework.git
* 直接在終端機輸入 `make `


## OS environment
* macOS
* linux

## 解決方法敘述
每個學生在自己進去實驗室的被助教教學的2秒，每1秒去確認一次外面在等待的學生人數(numberOfOccupiedChairs)

=> waitingTime = waitingTime + numberOfOccupiedChairs
來算出每位學生在實驗室外+實驗室內的時間  

最後將waitingTime除以waitingStudents(進實驗室的學生)之後再加上2（學生自己在實驗室內的時間）得到平均時間

## problem
* 寫MAKEFILE的時候，解決linux 無法link到外部模組 https://www.ptt.cc/bbs/C_and_CPP/M.1371650582.A.068.html https://stackoverflow.com/questions/10409032/why-am-i-getting-undefined-reference-to-sqrt-error-even-though-i-include-math
