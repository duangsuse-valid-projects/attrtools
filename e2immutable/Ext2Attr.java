import java.io.Closeable;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Scanner;

/** Ext2 like filesystem file attribute querying library
 *
 * @author duangsuse
 * @see "https://github.com/duangsuse/attrtools"
 * @see "e2imutable.c"
 * @since 1.0
 * @version 1.0
 */
public class Ext2Attr implements Closeable {
    /** Error string field */
    @SuppressWarnings("WeakerAccess")
    public static String error = "";

    /** E2IM executable path */
    @SuppressWarnings("WeakerAccess")
    public String lib_path = "e2im.so";

    /** Shell executable path */
    @SuppressWarnings("WeakerAccess")
    public String su_path = "su";

    /** Shell process instance */
    @SuppressWarnings("WeakerAccess")
    public Process shell = null;

    /** Shell stdin */
    @SuppressWarnings("WeakerAccess")
    public PrintStream stdin = null;

    /** Shell stdout */
    @SuppressWarnings("WeakerAccess")
    public Scanner stdout = null;

    /** Shell stderr */
    @SuppressWarnings("WeakerAccess")
    public Scanner stderr = null;

    /** Default constructor */
    public Ext2Attr(String su_path, String lib_path) {
        this.su_path = su_path;
        this.lib_path = lib_path;
        connect();
    }

    public Ext2Attr(String lib_path) {
        this.lib_path = lib_path;
        connect();
    }

    public Ext2Attr() {
        connect();
    }

    /** Disconnect form shell */
    public void close() {
        stdin.println("exit");
        stdin.flush();
        try {
            shell.waitFor();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /** Connect with shell */
    @SuppressWarnings("WeakerAccess")
    public void connect() {
        try {
            shell = Runtime.getRuntime().exec(this.su_path);
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (shell == null)
            return;

        stdin = new PrintStream(shell.getOutputStream());
        stdout = new Scanner(new DataInputStream(shell.getInputStream()));
        stderr = new Scanner(new DataInputStream(shell.getErrorStream()));
        stdout.useDelimiter("_");
    }

    /** Connected with shell? */
    public boolean connected() {
        return shell != null && stdin != null;
    }

    /** Query file attribute
     *
     * @return 0 for no attribute<p>
     *     1 for +i<p>
     *     2 for +a<p>
     *     3 for +i+a<p>
     *     -1 for no file<p>
     *
     * @throws RuntimeException reading attr fails
     */
    public byte query(String path) throws RuntimeException {
        String command = String.format("%1$s @ %2$s;printf $?_", lib_path, path);
        stdin.println(command);
        stdin.flush();

        switch (stdout.nextInt()) {
            case 0:
                return 0;
            case 255:
                return -1;
            case 254:
                error = stderr.nextLine();
                throw new RuntimeException(error);
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            default:
                throw new RuntimeException("Cannot parse status");
        }
    }

    /** File attribute +i
     *
     * @return 0 for changed<p>
     *     1 for unchanged<p>
     *     -1 for no file<p>
     *
     * @throws RuntimeException change attr fails
     */
    public byte addi(String path) throws RuntimeException {
        String command = String.format("%1$s + %2$s;printf $?_", lib_path, path);
        stdin.println(command);
        stdin.flush();

        switch (stdout.nextInt()) {
            case 0:
                return 0;
            case 1:
                return 1;
            case 255:
                return -1;
            case 254:
                error = stderr.nextLine();
                throw new RuntimeException(error);
            default:
                throw new RuntimeException("Cannot parse status");
        }
    }

    /** File attribute -i
     *
     * @return 0 for changed<p>
     *     1 for unchanged<p>
     *     -1 for no file<p>
     *
     * @throws RuntimeException change attr fails
     */
    public byte subi(String path) throws RuntimeException {
        String command = String.format("%1$s - %2$s;printf $?_", lib_path, path);
        stdin.println(command);
        stdin.flush();

        switch (stdout.nextInt()) {
            case 0:
                return 0;
            case 1:
                return 1;
            case 255:
                return -1;
            case 254:
                error = stderr.nextLine();
                throw new RuntimeException(error);
            default:
                throw new RuntimeException("Cannot parse status");
        }
    }
}
