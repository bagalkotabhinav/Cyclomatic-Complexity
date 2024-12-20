int complexFunction(int x, int y) {
    int result = 0;
    
    if (x > 0) {
        if (y > 0) {
            result = x + y;
        } else {
            result = x - y;
        }
    } else {
        for (int i = 0; i < y; i++) {
            result += i;
        }
    }
    
    while (result > 100) {
        result /= 2;
    }
    
    return result;
}

int main(){
    return 0;
}