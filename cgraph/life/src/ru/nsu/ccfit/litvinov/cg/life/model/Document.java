package ru.nsu.ccfit.litvinov.cg.life.model;

import java.io.*;
import java.util.Observable;

public class Document extends MidObservable {
    private Field field;
    boolean saved = true;
    private FieldDisplayProps dprops;
    public FieldDisplayProps getDprops() {
        return dprops;
    }

    public boolean isSaved() {
        return saved;
    }

    public Field getField() {
        return field;
    }

    @Override
    public void update(Observable o, Object arg) {
        super.update(o, arg);
        saved = false;
    }

    public Document(Field field, FieldDisplayProps dprops) {
        this.field = field;
        this.dprops = dprops;
        dprops.addObserver(this);
        field.addObserver(this);
    }

    private StringBuilder sbuilder = new StringBuilder();
    public void save(OutputStream os) throws IOException {
        BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(os));
        bw.write(field.getmSize() + " " + field.getnSize() + "\n");
        bw.write(dprops.getLineThickness() + "\n");
        bw.write(dprops.getCellHeight() + "\n");
        int aliveCnt = 0;
        sbuilder.setLength(0);
        boolean[][] bfield = field.getField();

        for (int i = 0; i < field.getmSize(); ++i) {
            for (int j = 0; j < field.getnSize(); ++j) {
                if (bfield[i][j] == Field.ALIVE) {
                    ++aliveCnt;
                    sbuilder.append(i);
                    sbuilder.append(' ');
                    sbuilder.append(j);
                    sbuilder.append('\n');
                }
            }
        }
        bw.write(aliveCnt + "\n");
        bw.write(sbuilder.toString());
        bw.flush();
        saved = true;
    }

    public static Document open(InputStream is) throws IOException, BadFileFormatException {
        BufferedReader br = new BufferedReader(new InputStreamReader(is));
        int mSize, nSize, thickness, height;
        String tmp = br.readLine();
        if (tmp == null)
            throw new BadFileFormatException("Unexpected EOF");
        String[] spl = tmp.split(" ");
        if (spl.length != 2)
            throw new BadFileFormatException("Bad field count on line 1");
        try {
            mSize = Integer.parseInt(spl[0]);
            nSize = Integer.parseInt(spl[1]);
        } catch (NumberFormatException e) {
            throw new BadFileFormatException("Bad m n");
        }

        tmp = br.readLine();
        if (tmp == null)
            throw new BadFileFormatException("Unexpected EOF");
        try {
            thickness = Integer.parseInt(tmp);
        } catch (NumberFormatException e) {
            throw new BadFileFormatException("Bad line thickness");
        }

        tmp = br.readLine();
        if (tmp == null)
            throw new BadFileFormatException("Unexpected EOF");
        try {
            height = Integer.parseInt(tmp);
        } catch (NumberFormatException e) {
            throw new BadFileFormatException("Bad cell height");
        }

        int totAlive = 0;
        tmp = br.readLine();
        if (tmp == null)
            throw new BadFileFormatException("Unexpected EOF");
        try {
            totAlive = Integer.parseInt(tmp);
        } catch (NumberFormatException e) {
            throw new BadFileFormatException("Bad total 1s count");
        }

        Field fld = new Field(mSize, nSize, Rules.DEFAULT);

        int ci, cj;
        for (int i = 0; i < totAlive; ++i) {
            tmp = br.readLine();
            if (tmp == null)
                throw new BadFileFormatException("Unexpected EOF");
            spl = tmp.split(" ");
            if (spl.length != 2)
                throw new BadFileFormatException("Bad (i,j) pair length");
            try {
                ci = Integer.parseInt(spl[0]);
                cj = Integer.parseInt(spl[1]);
            } catch (NumberFormatException e) {
                throw new BadFileFormatException("Bad i or j");
            }
            fld.set(ci, cj);
        }

        return new Document(fld, new FieldDisplayProps(height, thickness));
    }
}
