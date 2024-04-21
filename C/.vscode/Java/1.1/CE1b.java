public class CE1b {
    public static void drawTriangle(int N){
        int i=0,j=0;
        for(i=0;i<N;i++){
            for(j=0;j<=i;j++){
                System.out.print("*");
            }
            System.out.print("\n");
        }
    }
    
    public static void main(String[] args){
        int N=10;
        drawTriangle(N);
    }
}
