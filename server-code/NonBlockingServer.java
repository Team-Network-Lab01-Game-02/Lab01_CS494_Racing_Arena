import java.io.*;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.*;

public class NonBlockingServer {
    private static final Integer portNumber = 6969;
    private static final Integer numPlayers = 3;
    private static final Integer startScore = 0;
    private static final Integer endScore = 10;
    private static final Integer maxConsecWrong = 2;

    private static ServerSocketChannel serverSocketChannel;

    private static final List<SocketChannel> socketChannels = new ArrayList<>();

    private static final Set<Integer> currentPlayers = new HashSet<>();

    private static final List<LineReader> readers = new ArrayList<>();
    private static final List<LineWriter> writers = new ArrayList<>();

    private static final List<String> names = new ArrayList<>();

    private static Boolean isCurrentPlayer(Integer player) {
        for (Integer currentPlayer : currentPlayers) {
            if (currentPlayer.equals(player)) {
                return true;
            }
        }

        return false;
    }

    private static Boolean allStatesLargerThan(List<Integer> states, Integer markState, Boolean includeNonCurrent) {
        if (includeNonCurrent) {
            for (Integer i = 0; i < numPlayers; ++i) {
                if (states.get(i) <= markState) {
                    return false;
                }
            }
        } else {
            for (Integer i : currentPlayers) {
                if (states.get(i) <= markState) {
                    return false;
                }
            }
        }

        return true;
    }

    private static void writeToAll(List<String> messages) {
        List<Integer> states = new ArrayList<>();
        for (Integer i = 0; i < numPlayers; ++i) {
            states.add(0);
        }

        while (true) {
            if (allStatesLargerThan(states, 1, false)) {
                break;
            }

            for (Integer i : currentPlayers) {
                switch (states.get(i)) {
                    case 0: {
                        Boolean writeDone = writers.get(i).writeLine(messages.get(i));
                        if (!writeDone) {
                            states.set(i, 1);
                        } else {
                            states.set(i, 2);
                        }
                        break;
                    }
                    case 1: {
                        Boolean writeDone = writers.get(i).keepWriting();
                        if (writeDone) {
                            states.set(i, 2);
                        }
                        break;
                    }
                }
            }
        }
    }

    private static void writeToAll(String message) {
        List<String> messages = new ArrayList<>();
        for (Integer i = 0; i < numPlayers; ++i) {
            messages.add(message);
        }

        writeToAll(messages);
    }

    private static List<String> readFromAll() {
        List<String> readResults = new ArrayList<>();
        for (Integer i = 0; i < numPlayers; ++i) {
            readResults.add("");
        }

        List<Integer> states = new ArrayList<>();
        for (Integer i = 0; i < numPlayers; ++i) {
            states.add(0);
        }

        while (true) {
            if (allStatesLargerThan(states, 0, false)) {
                break;
            }

            for (Integer i : currentPlayers) {
                switch (states.get(i)) {
                    case 0: {
                        String readResult = readers.get(i).readLine();
                        if (readResult == null) {
                            readers.get(i).readFromChannel();
                        } else {
                            readResults.set(i, readResult);
                            states.set(i, 1);
                        }

                        break;
                    }
                }
            }
        }

        return readResults;
    }

    private static Boolean nameIsDuplicate(String name) {
        for (String currentName : names) {
            if (currentName.equals(name)) {
                return true;
            }
        }

        return false;
    }

    private static void usernameHandling() {
        List<Integer> states = new ArrayList<>();
        for (Integer i = 0; i < numPlayers; ++i) {
            states.add(0);
        }

        while (true) {
            if (allStatesLargerThan(states, 5, true)) {
                break;
            }

            if (currentPlayers.size() < numPlayers) {
                try {
                    SocketChannel socketChannel = serverSocketChannel.accept();

                    if (socketChannel != null) {
                        Integer curIndex = currentPlayers.size();

                        socketChannel.configureBlocking(false);
                        LineReader lineReader = new LineReader(socketChannel);
                        LineWriter lineWriter = new LineWriter(socketChannel);
                        lineReader.readFromChannel();

                        socketChannels.add(socketChannel);
                        readers.add(lineReader);
                        writers.add(lineWriter);

                        currentPlayers.add(curIndex);

                        states.set(curIndex, 1);
                    }
                } catch (Exception e) {
                    System.out.println("Username handling exception: " + e.getMessage());
                }
            }

            for (Integer i = 0; i < numPlayers; ++i) {
                switch (states.get(i)) {
                    case 1: {
                        Boolean writeDone = writers.get(i).writeLine("username_request");
                        if (!writeDone) {
                            states.set(i, 2);
                        } else {
                            states.set(i, 3);
                        }
                        break;
                    }
                    case 2: {
                        Boolean writeDone = writers.get(i).keepWriting();
                        if (writeDone) {
                            states.set(i, 3);
                        }
                        break;
                    }
                    case 3: {
                        String name = readers.get(i).readLine();
                        if (name == null) {
                            readers.get(i).readFromChannel();
                        } else {
                            if (nameIsDuplicate(name)) {
                                Boolean writeDone = writers.get(i).writeLine("username_request_status,Failure");
                                if (!writeDone) {
                                    states.set(i, 4);
                                } else {
                                    states.set(i, 3);
                                }
                            } else {
                                names.set(i, name);
                                Boolean writeDone = writers.get(i).writeLine("username_request_status,Success");
                                if (!writeDone) {
                                    states.set(i, 5);
                                } else {
                                    states.set(i, 6);
                                }
                            }
                        }
                        break;
                    }
                    case 4: {
                        Boolean writeDone = writers.get(i).keepWriting();
                        if (writeDone) {
                            states.set(i, 3);
                        }
                        break;
                    }
                    case 5: {
                        Boolean writeDone = writers.get(i).keepWriting();
                        if (writeDone) {
                            states.set(i, 6);
                        }
                        break;
                    }
                }
            }
        }
    }

    public static void main(String[] args) {
        // TODO: Name duplicate bug

        try {
            serverSocketChannel = ServerSocketChannel.open();
            serverSocketChannel.socket().bind(new InetSocketAddress(6969));
            serverSocketChannel.configureBlocking(false);

            for (Integer i = 0; i < numPlayers; ++i) {
                names.add("");
            }

            List<Integer> scores = new ArrayList<>();

            for (Integer i = 0; i < numPlayers; ++i) {
                scores.add(startScore);
            }

            List<Integer> consecWrong = new ArrayList<>();

            for (Integer i = 0; i < numPlayers; ++i) {
                consecWrong.add(0);
            }

            /*
            for (Integer i = 0; i < numPlayers; ++i) {
                SocketChannel socketChannel = serverSocketChannel.accept();
                socketChannel.configureBlocking(false);
                LineReader lineReader = new LineReader(socketChannel);
                LineWriter lineWriter = new LineWriter(socketChannel);
                lineReader.readFromChannel();

                socketChannels.add(socketChannel);
                readers.add(lineReader);
                writers.add(lineWriter);

                currentPlayers.add(i);
            }
            */

            // writeToAll("username_request");
            usernameHandling();
            readFromAll();

            String usernameIdMapping = "username_id_mapping,start";
            for (Integer i = 0; i < numPlayers; ++i) {
                usernameIdMapping += "," + i + " " + names.get(i);
            }
            usernameIdMapping += ",end";
            writeToAll(usernameIdMapping);
            readFromAll();

            String scoreInfo = "score_info," + startScore + "," + endScore;
            writeToAll(scoreInfo);
            readFromAll();

            Random rand = new Random();

            while (true) {
                writeToAll("wait_ready");
                readFromAll();

                Integer num1 = rand.nextInt(10);
                Integer num2 = rand.nextInt(10);

                Integer correctAnswer = num1 + num2;

                String question = "question," + num1 + " + " + num2;

                writeToAll(question);

                List<Boolean> correct = new ArrayList<>();

                Integer fastestIndex = -1;
                Long fastestDuration = Long.MAX_VALUE;

                Integer loseCount = 0;

                List<Integer> removedPlayers = new ArrayList<>();

                List<String> questionResponses = readFromAll();

                for (Integer i : currentPlayers) {
                    List<String> questionResponseTokens = Arrays.asList(questionResponses.get(i).split(","));
                    Integer answer = Integer.parseInt(questionResponseTokens.get(0));
                    Long duration = Long.parseLong(questionResponseTokens.get(1));

                    if (answer.equals(correctAnswer)) {
                        correct.add(true);

                        Integer currentScore = scores.get(i);
                        scores.set(i, currentScore + 1);

                        consecWrong.set(i, 0);
                    } else {
                        correct.add(false);

                        Integer currentScore = scores.get(i);
                        scores.set(i, currentScore - 1);

                        if (scores.get(i) < 0) {
                            scores.set(i, 0);
                        }

                        loseCount += 1;

                        Integer curConsecWrong = consecWrong.get(i);
                        consecWrong.set(i, curConsecWrong + 1);

                        if (consecWrong.get(i) > maxConsecWrong) {
                            removedPlayers.add(i);
                        }
                    }

                    if (duration < fastestDuration) {
                        fastestIndex = i;
                        fastestDuration = duration;
                    }
                }

                Integer currentScoreFastest = scores.get(fastestIndex);
                scores.set(fastestIndex, currentScoreFastest + loseCount - 1);

                List<String> answerInfos = new ArrayList<>();
                for (Integer i = 0; i < numPlayers; ++i) {
                    answerInfos.add("");
                }

                for (Integer i : currentPlayers) {
                    String answerInfo = "answer_info,";

                    answerInfo += correctAnswer;

                    if (correct.get(i)) {
                        answerInfo += ",Correct";
                    } else {
                        answerInfo += ",Wrong";
                    }

                    if (i.equals(fastestIndex)) {
                        answerInfo += ",Fastest";
                    } else {
                        answerInfo += ",Not fastest";
                    }

                    boolean remain = true;

                    for (Integer j : removedPlayers) {
                        if (j.equals(i)) {
                            remain = false;
                            break;
                        }
                    }

                    if (remain) {
                        answerInfo += ",Remain";
                    } else {
                        answerInfo += ",Remove";
                    }

                    answerInfo += ",start";
                    for (Integer j : removedPlayers) {
                        answerInfo += "," + j;
                    }
                    answerInfo += ",end";

                    answerInfo += ",start";
                    for (Integer j : currentPlayers) {
                        answerInfo += "," + j + " " + scores.get(j);
                    }
                    answerInfo += ",end";

                    answerInfos.set(i, answerInfo);
                }

                writeToAll(answerInfos);
                readFromAll();

                boolean cont = true;

                for (Integer i : currentPlayers) {
                    if (scores.get(i) >= endScore) {
                        cont = false;
                        break;
                    }
                }

                if (cont) {
                    writeToAll("continue,y");
                    readFromAll();
                } else {
                    writeToAll("continue,n");
                    readFromAll();
                    break;
                }

                for (Integer i : removedPlayers) {
                    currentPlayers.remove(i);
                }
            }

            /*
            String name;

            lineReader.readFromChannel();

            while (true) {
                String line = lineReader.readLine();

                if (line == null) {
                    lineReader.readFromChannel();
                } else {
                    name = line;
                    break;
                }
            }

            boolean writeDone = lineWriter.writeLine("Hello " + name);
            while (!writeDone) {
                writeDone = lineWriter.keepWriting();
            }
            */

            for (SocketChannel socketChannel : socketChannels) {
                socketChannel.close();
            }

            serverSocketChannel.close();
        } catch (IOException e) {
            System.out.println("Exception: " + e.getMessage());
        }

    }
}

class LineReader {
    private SocketChannel channel;
    private ByteBuffer buf;
    private StringBuilder curStr;

    public LineReader(SocketChannel channel, ByteBuffer buf) {
        this.channel = channel;
        this.buf = buf;
        curStr = new StringBuilder();
    }

    public LineReader(ByteBuffer buf) {
        this.buf = buf;
        curStr = new StringBuilder();
    }

    public LineReader(SocketChannel channel) {
        this.channel = channel;
        this.buf = ByteBuffer.allocate(1024);
        curStr = new StringBuilder();
    }

    public void readFromChannel() {
        try {
            channel.read(buf);
        } catch (Exception e) {
            System.out.println("Line reader exception: " + e.getMessage());
        }

        buf.flip();
    }

    public void flip() {
        buf.flip();
    }

    public void clear() {
        buf.clear();
    }

    public String readLine() {
        while (buf.hasRemaining()) {
            Character curChar = (char) buf.get();

            if (curChar.equals('\n')) {
                String outStr = curStr.toString();
                curStr = new StringBuilder();

                return outStr;
            }

            curStr.append(curChar);
        }

        buf.clear();

        return null;
    }
}

class LineWriter {
    private SocketChannel channel;
    private ByteBuffer buf;

    public LineWriter(SocketChannel channel) {
        this.channel = channel;
        this.buf = ByteBuffer.allocate(1024);
    }

    public boolean writeLine(String line) {
        line += "\n";
        buf.put(line.getBytes());
        buf.flip();

        try {
            channel.write(buf);
        } catch (Exception e) {
            System.out.println("Line writer exception: " + e.getMessage());
        }

        Boolean writeDone = !buf.hasRemaining();

        if (writeDone) {
            buf.clear();
            // buf.flip();
        }

        return writeDone;
    }

    public boolean keepWriting() {
        try {
            channel.write(buf);
        } catch (Exception e) {
            System.out.println("Line writer exception: " + e.getMessage());
        }

        Boolean writeDone = !buf.hasRemaining();

        if (writeDone) {
            buf.clear();
            // buf.flip();
        }

        return writeDone;
    }
}
