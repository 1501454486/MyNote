/*定义一个函数，返回较大的数*/
public class def_fun {
    public static int larger(int x, int y){
        if(x > y){
            return x;
        }
        return y;
    }

    public static void main(String[] args){
        System.out.println(larger(10, 0));
    }
}