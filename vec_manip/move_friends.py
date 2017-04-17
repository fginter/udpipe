import lwvlib

if __name__=="__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Train')
    parser.add_argument('--orig', required=True, help='A (.bin) embedding file before training.')
    parser.add_argument('--trained', required=True, help='A (.bin) embedding file after training (produced by tools/extract_embeddings).')
    args = parser.parse_args()

    orig_m=lwvlib.load(args.orig)
    trained_m=lwvlib.load(args.trained)

    df=orig_m.vectors-trained_m.vectors
    print(df)
