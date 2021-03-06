import java.net.InetAddress;

public class PongPayLoad {
    ByteArray byteArray;

    PongPayLoad() {
        byteArray = new ByteArray(16);
    }

    PongPayLoad(byte[] buff, int offset) {
        byte[] data = new byte[16];
        System.arraycopy(buff, offset, data, 0, 16);
        byteArray = new ByteArray(data);
    }

    PongPayLoad(byte[] buff) {
        byteArray = new ByteArray(buff);
    }

    public void setPort(int port) {
        byteArray.setBytes(port, 0);
    }

    public void setIP(String IP) {
        try {
            InetAddress ip = InetAddress.getByName(IP);
            byteArray.setBytes(ip.getAddress(), 4);
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public void setNumberOfFiles(int numberOfFiles) {
        byteArray.setBytes(numberOfFiles, 8);
    }

    public void setNumberOfBytes(int numberOfBytes) {
        byteArray.setBytes(numberOfBytes, 12);
    }

    public int getPort() {
        return byteArray.getInteger(0);
    }

    public String getIP() {
        try {
            InetAddress ip = InetAddress.getByAddress(byteArray.getBytes(4, 8));
            return ip.getHostAddress();
        } catch (Exception e) {
            System.out.println(e);
        }
        return "";
    }

    public int getNumberOfFiles() {
        return byteArray.getInteger(8);
    }

    public int getNumberOfBytes() {
        return byteArray.getInteger(12);
    }

    public void show() {
        System.out.println(getPort() + "\t" + getIP() + "\t" + getNumberOfFiles() + "\t" + getNumberOfBytes());
    }
}
