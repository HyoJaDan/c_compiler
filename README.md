이 코드는 SysY라는 c언어의 부분집합인 언어를 컴파일하는 컴파일러입니다. 코드는 c++로 작성되었습니다. 이 컴파일러는 크게 형태소 분석, 구문 분석, 의미 분석 및 코드 생성기로 구분되어 있습니다. 개발 언어는 c++17로 작성하였습니다.<br>

### 파일

1. 컴파일을 위한 제 코드는  `compile.cpp` 파일에 있습니다.<br>
2. 컴파일 하고자 하는 코드는 `testfile.txt`에 넣으시면 됩니다. 문법 요구사항을 준수하는 파일과, 준수하지 않은 파일이 모두 있고, 문법에 오류가 있다면, `error.txt`에 오류를 생성합니다. <br>
3. 입력 하고자 하는 코드는 `input.txt` 파일에 추가하시면 됩니다. <br>
4. 문법에 의해 구문분석된 파일은 `output.txt` 파일에 추가됩니다.<br>
5. 문법에 오류가 있다면, `error.txt`에 오류를 생성합니다. <br>
6. 코드 생성이 완료된 부분, 즉 printf로 출력이 되는 코드들은 `pcoderesult.txt` 파일에 추가됩니다.<br>

### 빠른 코드실행

문서를 읽지 않고, 빠르게 실행시키고 싶으시다면 마지막 부분인 코드 생성 부분의 코드를 가져와서, `input.txt`와 `testfile.txt`에 위에 업로드해둔 `‘예시’` 파일의 값을 각각 하나씩 넣고, 코드 실행 결과인 `pcoderesult.txt`와 문법 분석 결과인 `output.txt`, 에러는`error.txt`의 결과를 보시면 됩니다.<br>
https://drive.google.com/file/d/1B10-Q9Nrx77F8sw-5TjSa0JLthWqOfJ1/view?usp=drive_link<br>
이 링크를 클릭하시면 자세한 설명 문서를 볼수 있습니다. <br>
설명 문서.pdf를 보셔도 됩니다.
